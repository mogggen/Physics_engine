#include "config.h"
#include "imgui.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace Display;
namespace Example
{
	std::pair<V3, V3> findAABB(MeshResource& mesh, M4 modelMatrix)
	{
		V3 current = (modelMatrix * V4(mesh.min, 1)).toV3();
		std::pair<V3, V3> ret = {current, current};
		float data[6] = { mesh.min[0], mesh.max[0], mesh.min[1], mesh.max[1], mesh.min[2], mesh.max[2] };


        for (size_t i = 1; i < 8; i++)
        {
            current = ((modelMatrix) * V4(data[i / 4], data[2 + (i / 2) % 2], data[4 + i % 2], 1)).toV3();
			for	(size_t j = 0; j < 3; j++)
			{
				if (current[j] < ret.first[j]) ret.first[j] = current[j];
				if (current[j] > ret.second[j]) ret.second[j] = current[j];
			}
        }
		return ret;
	}

	inline const V3 find_AABB_intersection(Ray& ray, MeshResource& mesh)
	{
		Plane left_plane(V3(mesh.min[0], 0, 0), V3(mesh.min[0], 0, 0));
		Plane right_plane(V3(mesh.max[0], 0, 0), V3(mesh.max[0], 0, 0));

		Plane bottom_plane(V3(0, mesh.min[1], 0), V3(0, mesh.min[1], 0));
		Plane top_plane(V3(0, mesh.max[1], 0), V3(0, mesh.max[1], 0));

		Plane front_plane(V3(0, 0, mesh.max[2]), V3(0, 0, mesh.max[2]));
		Plane back_plane(V3(0, 0, mesh.min[2]), V3(0, 0, mesh.min[2]));

		V3 res_left = ray.intersect(left_plane);
		V3 res_two = ray.intersect(right_plane);

		V3 res_bottom = ray.intersect(bottom_plane);
		V3 res_top = ray.intersect(top_plane);

		V3 res_front = ray.intersect(front_plane);
		V3 res_back = ray.intersect(back_plane);

		std::vector<V3> tt;
		tt.push_back(res_left);
		tt.push_back(res_two);

		tt.push_back(res_top);
		tt.push_back(res_bottom);

		tt.push_back(res_front);
		tt.push_back(res_back);

		return ray.minDist(tt);
	}

	struct Face
	{
		std::vector<V3> vertices;
		V3 normal;
	};



	// Function to calculate the projection of an object onto a given axis
	float ProjectOntoAxis(const std::vector<Face>& faces, const V3& axis) {
		float min = FLT_MAX;
		float max = -FLT_MAX;

		for (const Face& face : faces)
		{
			for (const V3& vertex : face.vertices) {
				float projection = Dot(vertex, axis);
				if (projection < min) {
					min = projection;
				}
				if (projection > max) {
					max = projection;
				}
			}
		}

		return max - min;
	}

	// Function to calculate the penetration point along the axis
	const V3 CalculatePenetrationPoint(
		const std::vector<Face>& i_faces,
		const std::vector<Face>& j_faces,
		const V3& axis)
	{

		std::vector<Face> i_best;
		{
			float maxProjection = -FLT_MAX;

			for (const Face& face : i_faces)
			{
				for (const V3& vertex : face.vertices) {
					float projection = Dot(vertex, axis);
					if (projection > maxProjection) {
						maxProjection = projection;
						i_best = {face};
					}
					else if (projection == maxProjection)
					{

					}
				}
			}

		}
		
		std::vector<Face> j_best;
		{
			float maxProjection = -FLT_MAX;

			for (const Face& face : j_faces)
			{
				for (const V3& vertex : face.vertices) {
					float projection = Dot(vertex, axis);
					if (projection > maxProjection) {
						maxProjection = projection;
						j_best = {face};
					}
				}
			}
		}

		if (i_best.size() != 1 || j_best.size() != 1)
		{
			// let's handle if more than 2 triangle faces collide here...
			return;
		}
			

		if (i_best[0].vertices.size() == 1) return i_best[0].vertices[0];
		if (j_best[0].vertices.size() == 1) return j_best[0].vertices[0];
		
		if (i_best[0].vertices.size() == 2) return findAverage(i_best[0].vertices);
		if (j_best[0].vertices.size() == 2) return findAverage(j_best[0].vertices);
		
		if (i_best[0].vertices.size() == 2 && j_best[0].vertices.size() == 2)
		{
			Ray line1(
				i_best[0].vertices[0],
				i_best[0].vertices[1] -
				i_best[0].vertices[0]
			);
			Ray line2(
				j_best[0].vertices[0],
				j_best[0].vertices[1] -
				j_best[0].vertices[0]
			);
			
			V3 delta = line1.origin - line2.origin;
			float t = (Dot(line2.dir, delta) - Dot(line1.dir, line2.dir)) / Dot(line1.dir, line1.dir);
			V3 closestPointOnLine1 = line1.origin + line1.dir * t;
			V3 closestPointOnLine2 = line2.origin + line2.dir * t;

			return findAverage({ closestPointOnLine1, closestPointOnLine2 });
		}

		const V3 mergeSums = [&](std::vector<Face>, std::vector<Face>) -> V3 {
			std::vector<V3> temp_i(i_best.size());
			for (Face& f : i_best)
			{
				temp_i.insert(temp_i.end(), f.vertices.begin(), f.vertices.end());
			}
			std::vector<V3> temp_j(j_best.size());
			for (Face& f : j_best)
			{
				temp_j.insert(temp_j.end(), f.vertices.begin(), f.vertices.end());
			}
			return findAverage(temp_j);
		}(i_best, j_best);

		return mergeSums;
	}

	struct CollisionInfo {
		bool isColliding = false;
		float depth = 0.f;
		V3 polytope;
		V3 norm1;
		V3 norm2;
	};

	static CollisionInfo sat(const std::vector<Face>& i_vertices, const std::vector<Face>& j_vertices) {
		CollisionInfo collisionInfo;
		collisionInfo.isColliding = true;
		collisionInfo.depth = FLT_MAX;

		//V3 previous;

		for (const Face& i : i_vertices) {
			for (const Face& j : j_vertices) {
				
				V3 axis = Cross(i.normal, j.normal);

				float projection1 = ProjectOntoAxis(i_vertices, axis);
				float projection2 = ProjectOntoAxis(j_vertices, axis);
				float overlap = projection1 + projection2;

				if (overlap < 0.f) {
					collisionInfo.isColliding = false;
					return collisionInfo; // No collision
				}
				else if (overlap < collisionInfo.depth) {
					// Store penetration depth and the penetration points
					collisionInfo.depth = overlap;
					collisionInfo.polytope = CalculatePenetrationPoint(i_vertices, j_vertices, axis);
					collisionInfo.norm1 = i.normal;
					collisionInfo.norm2 = j.normal;
				}
			}
		}

		return collisionInfo; // Collision detected
	}


	const V3 collisionPoint() {
		std::vector<Face> face1, face2;

		std::vector<V3> res1;
		// averge of collision f1
		for (Face f : face1)
		{
			res1.push_back(findAverage(f.vertices));
		}

		std::vector<V3> res2;
		for (Face f : face2)
		{
			res2.push_back(findAverage(f.vertices));
		}

		res1.insert(res1.end(), res2.begin(), res2.end());
		return findAverage(res1);
	}

	void Print(V4 v)
	{
		std::cout << '(';
		for (size_t i = 0; i < 4; i++)
			std::cout << v.data[i] << (i == 3 ? ")\n" : ", ");
	}

	void Print(M4 m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			V4 v = m[i];
			Print(v);
		}
	}

	void magic()
	{
		std::vector<Face> fire;
		Example::MeshResource::LoadTeaPot(fire);

		std::vector<Face> water;
		Example::MeshResource::LoadTeaPot(water);

		auto getVertFromFaces =
		[&](std::vector<Face> name) -> std::vector<V3> {
			std::vector<V3> temp(name.size());
			for (Face& f : name)
			{
				temp.insert(temp.end(), f.vertices.begin(), f.vertices.end());
			}
			return temp;
		};

		apply_worldspace(getVertFromFaces(fire), Translate(V4(-0.5f, 0, 0)));
		apply_worldspace(getVertFromFaces(water), Translate(V4(0.5f, 0, 0)));

		
		CollisionInfo info = sat(fire, water);
		
		

		//for (const V3& point : intersectionPoints) {
		//	std::cout << "Intersection Point: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
		//}
	}

	void ExampleApp::Run()
	{
		magic();
		
	}
}