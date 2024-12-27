
#include "config.h"
#include "imgui.h"
#include "stb_image.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <cstring>
#include <chrono>
#ifdef __linux__
#include <unistd.h>
#endif
struct Actor;

static void Print(const M4& m)
{
	for (size_t i = 0; i < 4; i++)
	{
		V4 v = m[i];
		Print(v);
	}
}


using namespace Display;
namespace Example
{
	/**
	 */
	ExampleApp::ExampleApp()
	{
	}
	/**
	 */
	ExampleApp::~ExampleApp()
	{
	}
	/**
	 */

	static std::pair<V3, V3> findAABB(MeshResource &mesh, const M4& modelMatrix)
	{
		V3 current = (modelMatrix * V4(mesh.min, 1)).toV3();
		std::pair<V3, V3> ret = {current, current};
		float data[6] = {mesh.min[0], mesh.max[0], mesh.min[1], mesh.max[1], mesh.min[2], mesh.max[2]};

		for (size_t i = 1; i < 8; i++)
		{
			current = (modelMatrix*V4(data[i / 4], data[2 + (i / 2) % 2], data[4 + i % 2], 1)).toV3();
			for (size_t j = 0; j < 3; j++)
			{
				if (current[j] < ret.first[j])
					ret.first[j] = current[j];
				if (current[j] > ret.second[j])
					ret.second[j] = current[j];
			}
		}
		return ret;
	}

	inline static V3 find_AABB_intersection(Ray &ray, MeshResource &mesh)
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

	inline static const V3 ray_intersection(
		Ray &r,
		const M4& WorldSpaceTransform,
		std::vector<V3> &i_worldSpace_coords,
		std::vector<unsigned> &i_meshModel_indices,
		std::vector<V3> *normals = nullptr)
	{
		V3 closest_point = NAN_V3;
		bool isUndef = true;
		for (size_t i = 0; i < i_meshModel_indices.size(); i += 3)
		{
			if (i_worldSpace_coords.size() - 1 < i_meshModel_indices[i + 2])
				break;
			Plane p = Plane(NAN_V3, NAN_V3);

			V4 a4 = V4(/*Transpose*/ WorldSpaceTransform*V4(i_worldSpace_coords[i_meshModel_indices[i + 0]], 1));
			V4 b4 = V4(/*Transpose*/ WorldSpaceTransform*V4(i_worldSpace_coords[i_meshModel_indices[i + 1]], 1));
			V4 c4 = V4(/*Transpose*/ WorldSpaceTransform*V4(i_worldSpace_coords[i_meshModel_indices[i + 2]], 1));

			V3 a = V3(a4.x, a4.y, a4.z);
			V3 b = V3(b4.x, b4.y, b4.z);
			V3 c = V3(c4.x, c4.y, c4.z);
			if (false && normals != nullptr)
			{
				p.point = a;
				p.normal = (*normals)[i_meshModel_indices[i]];
			}
			else
			{
				p.point = a;
				p.normal = Cross(b - a, c - a);
			}
			float epsilon = 1e-5;
			V3 currentIntersect = r.intersect(p, epsilon);
			if (epsilon == 1337)
			{
				closest_point = V3();
				continue;
			}
			isUndef = false;

			if (IsPointInFace(currentIntersect, {a, b, c}))
			{
				if (isUndef)
				{
					V3 curr_dist = currentIntersect - r.origin;
					V3 best_dist = closest_point - r.origin;
					if (curr_dist.Length2() < best_dist.Length2())
					{
						closest_point = currentIntersect;
					}
				}
				else
				{
					closest_point = currentIntersect;
				}
			}
		}

		return closest_point;
	}
	static std::vector<V3> FindFaceIntersection(Face face1, Face face2)
	{
		std::vector<V3> intersectionPoints;
		wrapping_sort(face1.vertices);
		wrapping_sort(face2.vertices);
		for (size_t i = 0; i < face1.vertices.size(); ++i)
		{
			size_t j = (i + 1) % face1.vertices.size();
			V3 edge1_start = face1.vertices[i];
			V3 edge1_end = face1.vertices[j];
			for (size_t k = 0; k < face2.vertices.size(); ++k)
			{
				size_t l = (k + 1) % face2.vertices.size();
				V3 edge2_start = face2.vertices[k];
				V3 edge2_end = face2.vertices[l];
				V3 intersection;
				V3 dir1 = edge1_end - edge1_start;
				V3 dir2 = edge2_end - edge2_start;
				V3 d1 = edge2_start - edge1_start;
				V3 d2 = edge1_start - edge2_start;
				float cross = Cross(dir1, dir2).Length();

				if (cross < FLT_MARGIN)
				{
					continue;
				}

				float t1 = Length(Cross(d1, dir1)) / cross;
				float t2 = Length(Cross(d2, dir2)) / cross;

				if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
				{
					intersection = edge1_start + dir1 * t1;
					intersectionPoints.push_back(intersection);
				}
			}
		}

		return intersectionPoints;
	}
	static float ProjectOntoAxis(const std::vector<Face> &faces, const V3 &axis)
	{
		float min = FLT_MAX;
		float max = -FLT_MAX;

		for (const Face &face : faces)
		{
			for (const V3 &vertex : face.vertices)
			{
				float projection = Dot(vertex, axis);
				if (projection < min)
				{
					min = projection;
				}
				if (projection > max)
				{
					max = projection;
				}
			}
		}

		return max - min;
	}
	static V3 CalculatePenetrationPoint(
		const std::vector<Face> &i_faces,
		const std::vector<Face> &j_faces,
		const V3 &axis)
	{

		Face i_best;
		{
			float maxProjection = -FLT_MAX;

			for (const Face &face : i_faces)
			{
				for (const V3 &vertex : face.vertices)
				{
					float projection = Dot(vertex, axis);
					if (projection > maxProjection)
					{
						maxProjection = projection;
						i_best = face;
					}
				}
			}
		}

		Face j_best;
		{
			float maxProjection = -FLT_MAX;

			for (const Face &face : j_faces)
			{
				for (const V3 &vertex : face.vertices)
				{
					float projection = Dot(vertex, axis);
					if (projection > maxProjection)
					{
						maxProjection = projection;
						j_best = face;
					}
				}
			}
		}

		assert(i_best.vertices.size() != 0);
		assert(j_best.vertices.size() != 0);

		if (i_best.vertices.size() == 1)
			return i_best.vertices[0];
		if (j_best.vertices.size() == 1)
			return j_best.vertices[0];

		if (i_best.vertices.size() == 2)
			return findAverage(i_best.vertices);
		if (j_best.vertices.size() == 2)
			return findAverage(j_best.vertices);

		if (i_best.vertices.size() == 2 && j_best.vertices.size() == 2)
		{
			Ray line1(
				i_best.vertices[0],
				i_best.vertices[1] -
					i_best.vertices[0]);
			Ray line2(
				j_best.vertices[0],
				j_best.vertices[1] -
					j_best.vertices[0]);

			V3 delta = line1.origin - line2.origin;
			float t = (Dot(line2.dir, delta) - Dot(line1.dir, line2.dir)) / Dot(line1.dir, line1.dir);
			V3 closestPointOnLine1 = line1.origin + line1.dir * t;
			V3 closestPointOnLine2 = line2.origin + line2.dir * t;

			return findAverage({ closestPointOnLine1, closestPointOnLine2 });
		}
		return findAverage(FindFaceIntersection(i_best, j_best));
	}

	struct CollisionInfo
	{
		bool isColliding = false;
		float depth = 0.f;
		std::vector<V3> polytope;
		V3 norm1;
		V3 norm2;
	};

	static CollisionInfo sat(const std::vector<Face> &i_vertices, const std::vector<Face> &j_vertices)
	{
		CollisionInfo collisionInfo;
		collisionInfo.isColliding = true;
		collisionInfo.depth = FLT_MAX;

		for (const Face &i : i_vertices)
		{
			for (const Face &j : j_vertices)
			{

				V3 axis = Cross(i.normal, j.normal);

				float projection1 = ProjectOntoAxis(i_vertices, axis);
				float projection2 = ProjectOntoAxis(j_vertices, axis);
				float overlap = projection1 + projection2;

				if (overlap < 0.f)
				{
					collisionInfo.isColliding = false;
					return collisionInfo;
				}
				else if (overlap < collisionInfo.depth)
				{
					collisionInfo.depth = overlap;
					std::vector<V3> contactRegion = ClipFaceAgainstFace(i, j);
					collisionInfo.polytope = contactRegion;
					collisionInfo.norm1 = i.normal;
					collisionInfo.norm2 = j.normal;
				}
			}
		}

		return collisionInfo;
	}
	static void CollisionResponse(float e,float ma,float mb,M4& Ia,M4& Ib,V4 ra,V4 rb,V4 n,
	 V4 vai, V4 vbi, V4 wai, V4 wbi, V4& vaf, V4& vbf, V4& waf, V4& wbf) {
   M4 IaInverse = Inverse(Ia);
   V4 normal = Normalize(n);
   V4 angularVelChangeA  = normal;
   angularVelChangeA = Cross(angularVelChangeA,ra);
   angularVelChangeA = IaInverse * angularVelChangeA;
   V4 vaLinDueToR = Cross(angularVelChangeA, ra);
   float scalar = 1 / ma + Dot(vaLinDueToR, normal);
   M4 IbInverse = Inverse(Ib);
   V4 angularVelChangeB = normal;
   angularVelChangeB = Cross(angularVelChangeB, rb);
   angularVelChangeB = IbInverse * angularVelChangeB;
   V4 vbLinDueToR = Cross(angularVelChangeB, rb);
   scalar += 1 / mb + Dot(vbLinDueToR, normal);
   float Jmod = (e + 1) * Length(vai - vbi) / scalar;
   V4 J = normal * Jmod;
   vaf = vai - J * (1.f / ma);
   vbf = vbi - J * (1.f / mb);
   waf = wai - angularVelChangeA;
   wbf = wbi - angularVelChangeB;
 }

	bool
	ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;
		w = a = s = d = f = shift = space = false;
		window->GetSize(width, height);

		window->SetKeyPressFunction([this](int32 keycode, int32, int32 action, int32)
									{
			switch (keycode)
			{
			case GLFW_KEY_ESCAPE: window->Close(); break;
			case GLFW_KEY_W: w = action; break;
			case GLFW_KEY_S: s = action; break;
			case GLFW_KEY_A: a = action; break;
			case GLFW_KEY_D: d = action; break;
			case GLFW_KEY_F: f = action; break;
			case GLFW_KEY_G: showDebugRender = false; break;
			case GLFW_KEY_Z: showDebugRender = true; break;
			
			case GLFW_KEY_LEFT_SHIFT: shift = action; break;
			case GLFW_KEY_SPACE: space = action; break;
			default: break;
			} });

		window->SetMousePressFunction([this](int32 button, int32 action, int32)
									  { isPressed = button == GLFW_MOUSE_BUTTON_1 && action; });

		window->SetMouseMoveFunction([this](float64 x, float64 y)
									 {
			if (isPressed)
			{
				senseX = prevX + (0.002 * (x - width / 2));
				senseY = prevY + (0.002 * (y - height / 2));
				cubeProjectionViewTransform = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
				fireHydrantProjectionViewTransform = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
			} });

		if (this->window->Open())
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			std::vector<unsigned> fireIndices;
			std::vector<V3> fireCoords;
			std::vector<V2> fireTexels;
			std::vector<V3> fireNormals;
			std::vector<Vertex> fireVertices;
			std::vector<Face> fireFaces;
			fireHydrantMesh = MeshResource::Cube(fireIndices, 
				fireCoords, 
				fireNormals,
				fireVertices,
				fireFaces);
			fireHydrantMesh->indicesAmount = fireIndices;
			fireHydrantMesh->positions = fireCoords;
			fireHydrantMesh->texels = fireTexels;
			fireHydrantMesh->normals = fireNormals;
			fireHydrantMesh->vertices = fireVertices;
			fireHydrantMesh->faces = fireFaces;
			fireHydrantMesh->min = fireHydrantMesh->find_bounds().first;
			fireHydrantMesh->max = fireHydrantMesh->find_bounds().second;
			fireHydrantTexture = std::make_shared<TextureResource>("textures/cubepic.png");
			fireHydrantTexture->LoadFromFile();
			fireHydrantScript = std::make_shared<ShaderResource>();
			fireHydrantScript->LoadShader(fireHydrantScript->vs, fireHydrantScript->ps, "textures/vs.glsl", "textures/ps.glsl");
			std::shared_ptr<Actor> fireHydrantActor = std::make_shared<Actor>();
			fireHydrantActor->mass = 7;
			fireHydrantActor->elasticity = .5;
			texturedCube = std::make_shared<GraphicNode>(fireHydrantMesh, fireHydrantTexture, fireHydrantScript, fireHydrantActor);

			this->window->SetUiRender([this]()
									  { this->RenderUI(); });
			return true;
		}
		return false;
	}
	struct CollisionData
	{
		float penetration;
		V3 normal;
		V3 pointOnPlane;
	};

	struct SphereCollisionShape
	{
	};

	struct SphereCollisionDAT
	{
		bool AreColliding(CollisionData *out_coldata)
		{
			return false;
		}

		inline bool CheckCollisionAxis(
									   const V3 &axis, CollisionData &out_coldata)
		{

			V3 min1, min2, max1, max2;

			float A = Dot(axis, min1);
			float B = Dot(axis, max1);
			float C = Dot(axis, min2);
			float D = Dot(axis, max2);
			if (A <= C && B >= C)
			{
				out_coldata.normal = axis;
				out_coldata.penetration = C - B;
				out_coldata.pointOnPlane =
					max1 + out_coldata.normal * out_coldata.penetration;

				return true;
			}
			if (C <= A && D >= A)
			{
				out_coldata.normal = axis * -1.f;
				out_coldata.penetration = A - D;
				out_coldata.pointOnPlane =
					min1 + out_coldata.normal * out_coldata.penetration;

				return true;
			}
			return false;
		}
	};

	static void ConvertToFaces(std::shared_ptr<GraphicNode> &ith, std::vector<Face> &i_faces)
	{
		Face currFace;

		std::vector<Vertex> &verts = ith->getMesh()->vertices;
		for (size_t i = 0; i < verts.size(); i++)
		{
			if (currFace.vertices.empty())
			{
				currFace.vertices.push_back(verts[i].pos);
				currFace.normal = verts[i].normal;
			}
			if (currFace.normal == verts[i].normal)
			{
				currFace.vertices.push_back(verts[i].pos);
			}
			else if (currFace.vertices.empty())
			{
				currFace.vertices.push_back(verts[i].pos);
				currFace.normal = verts[i].normal;
			}
			else
			{
				i_faces.push_back(currFace);
				currFace = Face();
			}
		}
	}

	static void PointIsOnLine(const V3& PointIsOnLine, const V3& line, const float margin = 0.0001f)
	{

	}

	static void handle_collision(const std::shared_ptr<GraphicNode>& ith, const std::shared_ptr<GraphicNode>& jth, int frameIndex)
	{
		std::vector<V3>& i_vertices = ith->getMesh()->positions;
		apply_world_space(i_vertices, ith->actor->transform);
		std::vector<Face>& i_faces = ith->getMesh()->faces;
		apply_world_space(i_faces, ith->actor->transform);
		V3 i_cm = findAverage(i_vertices);
		std::vector<V3>& j_vertices = jth->getMesh()->positions;
		apply_world_space(j_vertices, jth->actor->transform);
		std::vector<Face>& j_faces = jth->getMesh()->faces;
		apply_world_space(j_faces, jth->actor->transform);
		V3 j_cm = findAverage(j_vertices);

		CollisionInfo& info = sat(i_faces, j_faces);


		if (!info.isColliding)
			return;
		else
			std::cout << "after index" << frameIndex << std::endl;

		if (Dot(info.norm1, info.norm2) > 0) std::cout << "" << std::endl;

		const float& m1 = ith->actor->mass;
		const float& m2 = jth->actor->mass;

		V4 r1 = findAverage(info.polytope) - i_cm;
		V4 r2 = findAverage(info.polytope) - j_cm;
		if (info.polytope.size() > 0)
		{
			std::cout << "AAAHHHH" << std::endl;
			exit(0);
		}
		const float& e1 = ith->actor->elasticity;
		const float& e2 = jth->actor->elasticity;

		V4& u1 = ith->actor->linearVelocity;
		V4& u2 = jth->actor->linearVelocity;

		M4& rot1 = ith->actor->rotation;
		M4& rot2 = jth->actor->rotation;
		float& o1 = ith->actor->angle;
		float& o2 = jth->actor->angle;
		float& w1 = ith->actor->angleVel;
		float& w2 = jth->actor->angleVel;

		assert(m1 > 0.f);
		assert(m2 > 0.f);

		 V4 relativeVelocity = u2 - u1;

		 V4 collisionNormal = Normalize(r2 * 1.f);
		 /*float impulseScalar = -(1 + e1) * Dot(relativeVelocity, collisionNormal) / Dot(collisionNormal, collisionNormal * (1 / m1 + 1 / m2));
		 V4 impulse = impulseScalar * collisionNormal;*/


		auto headOnCoef = 0.00005;

		const V4 v1 = headOnCoef * (i_cm - j_cm);
		const V4 v2 = headOnCoef * (j_cm - i_cm);
		r1 = (info.polytope.size() ? findAverage(info.polytope) : V4()) - i_cm;
		r2 = (info.polytope.size() ? findAverage(info.polytope) : V4()) - j_cm;
		

		V4 axis1 = Cross(r1, info.norm1);
		V4 axis2 = Cross(r2, info.norm2);
		
		V4 wa = axis1 * w1;
		V4 wb = axis2 * w2;
		
		V4 u1In = u1;
		V4 u2In = u2;

		CollisionResponse(0.5f, m1, m2, Translate(V4()), Translate(V4()), r1, r2, info.norm1, v1, v2, axis1 * w1, axis2 * w2,
			u1In, u2In, wa, wb);
		if (ith->actor->isDynamic)
			u1 = u1In;
		if (jth->actor->isDynamic)
			u2 = u2In;
		rot1 = ith->actor->isDynamic ? Rotation(axis1, Length(wa) * 0.0005) : rot1;
		rot2 = jth->actor->isDynamic ? Rotation(axis2, Length(wb) * 0.0005) : rot2;
		ith->getMesh()->positions = i_vertices;
		ith->getMesh()->faces = i_faces;
		jth->getMesh()->positions = j_vertices;
		jth->getMesh()->faces = j_faces;
		return;

		if (axis1.Length2())
			rot1 = Rotation(axis1, o1);

		if (axis2.Length2())
			rot2 = Rotation(axis2, o2);
	}

	void
	ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		time_t seed = time(nullptr);
		srand(seed);
		float x_rand = rand() / (float)RAND_MAX * 20.f - 10.f;
		float z_rand = rand() / (float)RAND_MAX * 20.f - 10.f;

		for (size_t i = 0; i < 2; i++)
		{
			auto node = std::make_shared<GraphicNode>(
				std::make_shared<MeshResource>(*texturedCube->getMesh().get()),
				texturedCube->getTexture(),
				texturedCube->getShader(),
				std::make_shared<Actor>(*texturedCube->getActor().get()));
			all_loaded.push_back(node);
			all_loaded[i]->actor = std::make_shared<Actor>();
			all_loaded[i]->actor->elasticity = 0.600f;
			all_loaded[i]->actor->mass = 2;
			all_loaded[i]->actor->isDynamic = false;

			all_loaded[i]->actor->transform = /*Scalar(V3(10.f, 0.1f, 10.f)) * */Translate(V4(i * 0.75f - 2.f, 0, 0));
		}

		{
			all_loaded[0]->actor->transform = Translate(V4(0, 1.f, 0)) * Rotation(V4(1, 1, 1), -0.35f);
			all_loaded[0]->actor->mass = 1;
			all_loaded[0]->actor->elasticity = 0.1f;
			all_loaded[0]->actor->isDynamic = true;
		}
		{
			all_loaded[1]->actor->transform = Translate(V4(0.f, -2.f, 0)) * Scalar(V3(1.f, 1.f, 1.f));
			all_loaded[1]->actor->mass = 100;
			all_loaded[1]->actor->elasticity = 0.1f;
		}
		float dt = 0.0001f;
		Ray ray(V3(FLT_MAX, FLT_MAX, FLT_MAX), V3(FLT_MAX, FLT_MAX, FLT_MAX));
		const float g = -9.806e-3f;
#define GRAVITY V3(0, g * 20, 0)

		Camera cam(90, (float)width / height, 0.01f, 1000.0f);
		cam.setPos(V4(0, 1, -3));
		cam.setRot(V4(0, 0, 1), M_PI);

		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);

		float camSpeed = .08f;

		for (const std::shared_ptr<GraphicNode> graphN : all_loaded)
		{
			std::pair<V3, V3> t = findAABB(*graphN->getMesh(), graphN->actor->transform);
			aabbs.push_back({t.first, t.second});
		}

		while (this->window->IsOpen())
		{

			auto start = std::chrono::high_resolution_clock::now();
			if (this->f)
			{
				auto gg = all_loaded[0]->actor->transform * all_loaded[0]->actor->rotation * Rotation(V4(1, 0, 0), -M_PI / 4);
				cam.setRot(V4(1, 0, 0, 0), -M_PI / 4);
			}
			else
			{
				cam.setPos(cam.getPos() + Normalize(V4((a - d), (shift - space), (w - s))) * camSpeed);
			}
			V3 rayOrigin = cam.getPos() * 1.f;


			for (size_t i = 0; i < all_loaded.size(); ++i)
			{

				AABB the;
				std::tie(the.min, the.max) = findAABB(*all_loaded[i]->getMesh(), all_loaded[i]->actor->transform);
				aabbs[i] = the;
			}

			/*std::vector<std::pair<size_t, size_t>> _ = aabbPlaneSweep(aabbs);
			for (std::pair<size_t, size_t> &ants : _)
			{*/
				std::shared_ptr<GraphicNode> ith = all_loaded[0];
				std::shared_ptr<GraphicNode> jth = all_loaded[1];

				handle_collision(all_loaded[0], all_loaded[1], frameIndex);
			//}
			for (std::shared_ptr<GraphicNode> node : all_loaded)
			{
				const float &m = node->actor->mass;
				if (node->actor->isDynamic)
				{
					node->actor->apply_force(m * GRAVITY * 0.1f, dt);
				}
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (std::shared_ptr<GraphicNode> sample_a : all_loaded)
			{
				const V4 color(1, 1, 1, 1);

				sample_a->getShader()->setM4(cam.pv(), "m4ProjViewPos");

				std::shared_ptr<ShaderResource> script = sample_a->getShader();

				sample_a->actor->update(3);
				M4 &wst = sample_a->actor->transform;
				if (showDebugRender)
				{

					MeshResource &m = *sample_a->getMesh();
					Debug::DrawBB(m, V4(0, 1, 1, 1), wst);

					std::pair<V3, V3> aabb = findAABB(m, wst);
					Debug::DrawAABB(aabb, V4(1, 0, 0, 1));
				}
				light.bindLight(script, cam.getPos());
				sample_a->DrawScene(cam.pv() * wst, color);
			}
			if (showDebugRender)
			{
				Debug::Render(cam.pv());
			}

			frameIndex++;
			this->window->Update();
			this->window->SwapBuffers();

			auto stop = std::chrono::high_resolution_clock::now();
			using ms = std::chrono::duration<float, std::milli>;
		}
	}

	void CollisionDetectionTests()
	{
		//point-against-face
		-3,-3 97,0 100,100
		Rotera med 45° längst +X-axeln vid hörn 97,0
		0,0 0,100 100,100 100,0

		97,0
		Z:0-100

		//face-against-face
		0,-99 0,1 100,1 100,-99
		0,0 0,100 100,100 100,0

		0,1 100,1

		//line-against-face
		input:
		5,0 10,-5 5,-10 0,-5
		0,0 0,10 10,10 10,0

		output:
		5,0

		z: 0-10

	}

	void ExampleApp::RenderUI()
	{
		bool show = true;
		ImGui::Begin("Panel", &show, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Checkbox("Debug Mode: ", &showDebugRender);
		ImGui::Text("frames: %d %.0f", frameIndex);
		ImGui::Text("det: %.5f", Determinant(all_loaded[0]->actor->rotation));

		for (size_t i = 0; i < all_loaded.size(); i++)
		{
			ImGui::Checkbox(std::string("Make dynamic cube " + std::to_string(i) + ": " + std::string(all_loaded[i]->actor->isDynamic ? "true" : "false")).c_str(), &all_loaded[i]->actor->isDynamic);
		}

		ImGui::End();
	}

}
