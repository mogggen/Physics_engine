//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
// 1. TODO: make sure that min max is correct
// 2. TODO: fix remaining bugs
// 3. TODO: Debug::DrawSphere()
#include "config.h"
#include "imgui.h"
#include "stb_image.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#ifdef __linux__
#include <unistd.h>
#endif
struct Actor;

using namespace Display;
namespace Example
{
	//------------------------------------------------------------------------------
	/**
	 */
	ExampleApp::ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	 */
	ExampleApp::~ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	 */


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

	const V3 ray_intersection(
		Ray& r,
		M4 WorldSpaceTransform,
		std::vector<V3>& i_worldSpace_coords,
		std::vector<unsigned>& i_meshModel_indices,
		std::vector<V3>* normals = nullptr)
	{
		V3 closest_point = NAN_V3;
		bool isUndef = true;
		//use the world space coordinates for the entire model
		for (size_t i = 0; i < i_meshModel_indices.size(); i += 3)
		{
			if (i_worldSpace_coords.size() - 1 < i_meshModel_indices[i + 2]) break;
			Plane p = Plane(NAN_V3, NAN_V3);
			//V3 a = i_worldSpace_coords[i_meshModel_indices[i + 0]];
			//V3 b = i_worldSpace_coords[i_meshModel_indices[i + 1]];
			//V3 c = i_worldSpace_coords[i_meshModel_indices[i + 2]];

			V4 a4 = V4(/*Transpose*/(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 0]], 1));
			V4 b4 = V4(/*Transpose*/(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 1]], 1));
			V4 c4 = V4(/*Transpose*/(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 2]], 1));

			V3 a = V3(a4.x, a4.y, a4.z);
			V3 b = V3(b4.x, b4.y, b4.z);
			V3 c = V3(c4.x, c4.y, c4.z);

			//create a plane for each of the faces check for intersections
			if (false && normals != nullptr) // always out of range, probably because the parser is broken
			{
				//if the model has normals use them instead
				p.point = a;
				p.normal = (*normals)[i_meshModel_indices[i]];
			}
			else
			{
				//calculate the normals and use them to create a plane
				p.point = a;
				p.normal = Cross(b - a, c - a);
			}

			// point in triangle with cross product
			float epsilon = 1e-5;
			V3 currentIntersect = r.intersect(p, epsilon);

			// TODO: undefined check
			if (epsilon == 1337)
			{
				closest_point = V3();
				continue;
			}
			isUndef = false;
			
			if (point_in_Face_3D(currentIntersect, { a, b, c }))
			{
				if (isUndef)
				{
					V3 curr_dist = currentIntersect - r.origin;
					V3 best_dist = closest_point - r.origin;

					// only update if it is the closest point on the mesh.
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
	
	struct Face
	{
		std::vector<V3> vertices;
		V3 normal;
	};

	// Function to find the intersection points between two faces
	std::vector<V3> FindFaceIntersection(Face face1, Face face2) {
		std::vector<V3> intersectionPoints;

		// sort in wrapping order
		wrapping_sort(face1.vertices);
		wrapping_sort(face2.vertices);

		// Iterate over the edges of the first face
		for (size_t i = 0; i < face1.vertices.size(); ++i) {
			size_t j = (i + 1) % face1.vertices.size();
			V3 edge1_start = face1.vertices[i];
			V3 edge1_end = face1.vertices[j];

			// Iterate over the edges of the second face
			for (size_t k = 0; k < face2.vertices.size(); ++k) {
				size_t l = (k + 1) % face2.vertices.size();
				V3 edge2_start = face2.vertices[k];
				V3 edge2_end = face2.vertices[l];

				// Calculate the intersection point of two line segments
				V3 intersection;
				V3 dir1 = edge1_end - edge1_start;
				V3 dir2 = edge2_end - edge2_start;
				V3 d1 = edge2_start - edge1_start;
				float cross = Cross(dir1, dir2).Length();

				if (cross < 0.0001) { // Parallel or collinear
					continue;
				}

				float t1 = Length(Cross(d1, dir2)) / cross;
				float t2 = Length(Cross(d1, dir1)) / cross;

				if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
					intersection = edge1_start + dir1 * t1;
					intersectionPoints.push_back(intersection);
				}
			}
		}

		return intersectionPoints;
	}

	void magic() {
		// Define two faces with their vertices
		Face face1;
		face1.vertices = {
			V3(0, 0, 0),
			V3(1, 0, 0),
			V3(1, 1, 0),
			V3(0, 1, 0)
		};

		Face face2;
		face2.vertices = {
			V3(0.5, 0.5, 0),
			V3(1.5, 0.5, 0),
			V3(1.5, 1.5, 0),
			V3(0.5, 1.5, 0)
		};

		// Find the intersection points
		std::vector<V3> intersectionPoints = FindFaceIntersection(face1, face2);

		// Print the intersection points
		
		//Intersection Point : (1, 0.5, 0)
		//Intersection Point : (0.5, 1, 0)
		//Intersection Point : (0.5, 1, 0)
		//Intersection Point : (0, 0.5, 0)
		
		for (const V3& point : intersectionPoints) {
			std::cout << "Intersection Point: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
		}
	}


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

		Face i_best;
		{
			float maxProjection = -FLT_MAX;

			for (const Face& face : i_faces)
			{
				for (const V3& vertex : face.vertices) {
					float projection = Dot(vertex, axis);
					if (projection > maxProjection) {
						maxProjection = projection;
						i_best = face;
					}
				}
			}

		}
		
		Face j_best;
		{
			float maxProjection = -FLT_MAX;

			for (const Face& face : j_faces)
			{
				for (const V3& vertex : face.vertices) {
					float projection = Dot(vertex, axis);
					if (projection > maxProjection) {
						maxProjection = projection;
						j_best = face;
					}
				}
			}
		}

		assert(i_best.vertices.size() == 0);
		assert(j_best.vertices.size() == 0);
		
		if (i_best.vertices.size() == 1) return i_best.vertices[0];
		if (j_best.vertices.size() == 1) return j_best.vertices[0];
		
		if (i_best.vertices.size() == 2) return findAverage(i_best.vertices);
		if (j_best.vertices.size() == 2) return findAverage(j_best.vertices);
		
		if (i_best.vertices.size() == 2 && j_best.vertices.size() == 2)
		{
			Ray line1(
				i_best.vertices[0],
				i_best.vertices[1] -
				i_best.vertices[0]
			);
			Ray line2(
				j_best.vertices[0],
				j_best.vertices[1] -
				j_best.vertices[0]
			);
			
			V3 delta = line1.origin - line2.origin;
			float t = (Dot(line2.dir, delta) - Dot(line1.dir, line2.dir)) / Dot(line1.dir, line1.dir);
			V3 closestPointOnLine1 = line1.origin + line1.dir * t;
			V3 closestPointOnLine2 = line2.origin + line2.dir * t;

			return findAverage({ closestPointOnLine1, closestPointOnLine2 });
		}
		return findAverage(FindFaceIntersection(i_best, j_best));
	}

	struct CollisionInfo {
		bool isColliding = false;
		float depth = 0.f;
		V3 polytope;
		V3 norm1;
		V3 norm2;
	};




	CollisionInfo sat(const std::vector<Face>& i_vertices, const std::vector<Face>& j_vertices) {
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

	/**
This function calulates the velocities after a 3D collision vaf, vbf, waf and wbf from information about the colliding bodies
@param double e coefficient of restitution which depends on the nature of the two colliding materials
@param double ma total mass of body a
@param double mb total mass of body b
@param M4 Ia inertia tensor for body a in absolute coordinates (if this is known in local body coordinates it must
                 be converted before this is called).
@param M4 Ib inertia tensor for body b in absolute coordinates (if this is known in local body coordinates it must
                 be converted before this is called).
@param V4 ra position of collision point relative to centre of mass of body a in absolute coordinates (if this is
                 known in local body coordinates it must be converted before this is called).
@param V4 rb position of collision point relative to centre of mass of body b in absolute coordinates (if this is
                 known in local body coordinates it must be converted before this is called).
@param V4 n normal to collision point, the line along which the impulse acts.
@param V4 vai initial velocity of centre of mass on object a
@param V4 vbi initial velocity of centre of mass on object b
@param V4 wai initial angular velocity of object a
@param V4 wbi initial angular velocity of object b
@param V4 vaf final velocity of centre of mass on object a
@param V4 vbf final velocity of centre of mass on object a
@param V4 waf final angular velocity of object a
@param V4 wbf final angular velocity of object b
*/
// void CollisionResponse(float e,float ma,float mb,M4 Ia,M4 Ib,V4 ra,V4 rb,V4 n,
//     V4 vai, V4 vbi, V4 wai, V4 wbi, V4 vaf, V4 vbf, V4 waf, V4 wbf) {
//   M4 IaInverse = Inverse(Ia);
//   V4 normal = Normalize(n);
//   V4 angularVelChangea  = normal; // start calculating the change in angular rotation of a
//   angularVelChangea.Cross(ra);
//   IaInverse.transform(angularVelChangea);
//   V4 vaLinDueToR = angularVelChangea.Cross(ra);  // calculate the linear velocity of collision point on a due to rotation of a
//   float scalar = 1 / ma + vaLinDueToR.Dot(normal);
//   M4 IbInverse = Inverse(Ib);
//   V4 angularVelChangeb = normal; // start calculating the change in angular rotation of b
//   angularVelChangeb.Cross(rb);
//   IbInverse.transform(angularVelChangeb);
//   V4 vbLinDueToR = angularVelChangeb.Cross(rb);  // calculate the linear velocity of collision point on b due to rotation of b
//   scalar += 1/mb + vbLinDueToR.Dot(normal);
//   float Jmod = (e+1)*(vai-vbi).Length()/scalar;
//   V4 J = normal * Jmod;
//   vaf = vai - J * (1.f / ma);
//   vbf = vbi - J * (1.f / mb);
//   waf = wai - angularVelChangea;
//   wbf = wbi - angularVelChangeb;
// }

	void Print(M4 m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			V4 v = m[i];
			std::cout << '(';
			for (size_t i = 0; i < 4; i++)
				std::cout << v.data[i] << (i == 3 ? ")\n" : ", ");
		}
	}

	bool
	ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;

		// assign ExampleApp variables
		w = a = s = d = shift = space = false;
		window->GetSize(width, height);

		window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
									{
			//deltatime
			switch (keycode)
			{
			case GLFW_KEY_ESCAPE: window->Close(); break;
			case GLFW_KEY_W: w = action; break;
			case GLFW_KEY_S: s = action; break;
			case GLFW_KEY_A: a = action; break;
			case GLFW_KEY_D: d = action; break;

			case GLFW_KEY_LEFT_SHIFT: shift = action; break;
			case GLFW_KEY_SPACE: space = action; break;
			} });

		window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
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
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			// MeshResource
			std::vector<unsigned> fireIndices;
			std::vector<V3> fireCoords;
			std::vector<V2> fireTexels;
			std::vector<V3> fireNormals;
			std::vector<Vertex> fireVertices;
			//TODO: fix this later if requried
			fireHydrantMesh = MeshResource::LoadObj("textures/cube.obj", fireIndices, fireCoords, fireTexels, fireNormals, fireVertices);
			fireHydrantMesh->indicesAmount = fireIndices;
			fireHydrantMesh->positions = fireCoords;
			fireHydrantMesh->texels = fireTexels;
			fireHydrantMesh->normals = fireNormals;
			fireHydrantMesh->vertices = fireVertices;
			fireHydrantMesh->min = fireHydrantMesh->find_bounds().first;
			fireHydrantMesh->max = fireHydrantMesh->find_bounds().second;

			// TextureResource
			fireHydrantTexture = std::make_shared<TextureResource>("textures/cubepic.png");
			fireHydrantTexture->LoadFromFile();

			// shaderResource
			fireHydrantScript = std::make_shared<ShaderResource>();
			fireHydrantScript->LoadShader(fireHydrantScript->vs, fireHydrantScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// Actor
			std::shared_ptr<Actor> fireHydrantActor = std::make_shared<Actor>();
			fireHydrantActor->mass = 7;
			fireHydrantActor->elasticity = .5;

			// GraphicNode
			texturedCube = std::make_shared<GraphicNode>(fireHydrantMesh, fireHydrantTexture, fireHydrantScript, fireHydrantActor);
			//all_loaded.push_back(texturedCube);

			// MeshResource
			std::vector<unsigned> cubeIndices;
			std::vector<V3> cubeCoords;
			std::vector<V2> cubeTexels;
			std::vector<V3> cubeNormals;
			std::vector<Vertex> cubeVertices;
			cubeMesh = MeshResource::LoadObj("textures/floorbox.obj", cubeIndices, cubeCoords, cubeTexels, cubeNormals, cubeVertices);
			cubeMesh->indicesAmount = cubeIndices;
			cubeMesh->positions = cubeCoords;
			cubeMesh->texels = cubeTexels;
			cubeMesh->normals = cubeNormals;
			cubeMesh->vertices = cubeVertices;

			cubeMesh->min = cubeMesh->find_bounds().first;
			cubeMesh->max = cubeMesh->find_bounds().second;
			
			cubeTexture = std::make_shared<TextureResource>("textures/red.png");
			cubeTexture->LoadFromFile();

			// shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->LoadShader(cubeScript->vs, cubeScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// Actor
			std::shared_ptr<Actor> cubeActor = std::make_shared<Actor>();
			cubeActor->mass = 300;
			cubeActor->elasticity = 0.3;
			cubeActor->isDynamic = false;

			// GraphicNode
			floor = std::make_shared<GraphicNode>(cubeMesh, cubeTexture, cubeScript, cubeActor);

			//all_loaded.push_back(floor);

			this->window->SetUiRender([this]()
									  { this->RenderUI(); });
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	 */

	void
	ExampleApp::Run()
	{
		magic();
		exit(0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		time_t f = time(nullptr);
		//std::cout << "seed: " << f << std::endl;
		srand(f);
		float x_rand = 0;//rand() / (float)RAND_MAX * 20.f - 10.f;
		float z_rand = rand() / (float)RAND_MAX * 20.f - 10.f;

		for (size_t i = 0; i < 2; i++)
		{
			const GraphicNode node = *texturedCube.get();
			all_loaded.push_back(std::make_shared<GraphicNode>(node));
			all_loaded[i]->actor = std::make_shared<Actor>();
			all_loaded[i]->actor->mass = 2;
			all_loaded[i]->actor->elasticity = 0.6;
			all_loaded[i]->actor->isDynamic = true;

			all_loaded[i]->actor->transform = Translate(V4(i * 0.75f, 0, 0));// *Rotation(V4(0, 0, 1), M_PI / 4.f)* Rotation(V4(0, 0, 1), z_rand);
			//all_loaded[i]->actor->linearVelocity = V3(-1e-3f, 1e-3f, 0);
		}

		all_loaded[0]->actor->mass = 1;
		all_loaded[0]->actor->elasticity = 0.5;
		all_loaded[0]->actor->linearVelocity = V3(-1e-3f, 0, 0);
		all_loaded[1]->actor->transform = Translate(V4(3.5f, 0, 0));

		all_loaded[1]->actor->transform = Translate(V4(-3.5f, 0, 0));
		all_loaded[1]->actor->mass = 100;
		all_loaded[1]->actor->elasticity = 0.5;
		all_loaded[1]->actor->linearVelocity = V3(1e-3f, -0, 0);
		//all_loaded[1]->actor->isDynamic = false;
		//all_loaded[1]->actor->transform = Translate(V4(1 * 0.75f, 3.f * 1, 0));
		// deltatime
		float dt = 0.01;
		Ray ray(V3(FLT_MAX, FLT_MAX, FLT_MAX), V3(FLT_MAX, FLT_MAX, FLT_MAX));

		// gravity
		const float g = -9.806e-3f;
#define GRAVITY V3(0, g, 0)

		Camera cam(90, (float)width / height, 0.01f, 1000.0f);
		cam.setPos(V4(0, 1, -3));
		cam.setRot(V4(0, 0, 1), M_PI);

		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);

		float camSpeed = .08f;

		

		// set identies
		floor->actor->transform = Translate(V4(0, -10.5f, 0));		
		
		for	(const std::shared_ptr<GraphicNode> g : all_loaded)
		{
			std::pair<V3, V3> t = findAABB(*g->getMesh(), g->actor->transform);
			aabbs.push_back({t.first, t.second});
		}

		while (this->window->IsOpen())
		{
			//--------------------ImGui section--------------------

			auto start = std::chrono::high_resolution_clock::now();
			// cube->actor->linearVelocity = V3(.05f * cos(frameIndex / 10.f)+0.001f, sin(frameIndex / 30.f) * 0.02f, 0);
			//--------------------math section--------------------
			cam.setPos(cam.getPos() + Normalize(V4((a - d), (shift - space), (w - s))) * camSpeed);
			V3 rayOrigin = cam.getPos() * 1.f;
			
			// effect of gravity
			
			if (isPressed)
			{
				glfwGetCursorPos(this->window->GetHandle(), &mouseDirX, &mouseDirY);
				// shot a ray

				V4 normalizedDeviceCoordinates(mouseDirX / width * 2 - 1, 1 - mouseDirY / height * 2, 1, 1);
				V4 mousePickingWorldSpace = Inverse(cam.pv()) * normalizedDeviceCoordinates;
				ray = Ray(rayOrigin, (mousePickingWorldSpace - rayOrigin).toV3());

				resultingHit = find_AABB_intersection(ray, *fireHydrantMesh);
				if (!isnan(resultingHit.x) || !isinf(resultingHit.y))
				if (!isnan(NAN/*resultingHit.data*/))
				{
					printf("%f, %f, %f\n", resultingHit.x, resultingHit.y, resultingHit.z);
					Debug::DrawLine(V4(resultingHit - V3(0, 3, 0), 1), V4(resultingHit - V3(0, 0, 0), 1), V4(1, 0, 0, 1));
					//Debug::DrawSquare(V4(resultingHit, 1));
				}
				resultingHit = ray_intersection(ray, fireHydrantWorldSpaceTransform, fireHydrantMesh->positions, fireHydrantMesh->indicesAmount, &(fireHydrantMesh)->normals);
			}
			//cube->actor->transform = Translate(V4(resultingHit, 1));

			for (size_t i = 0; i < all_loaded.size(); i++)
			{
				std::pair<V3, V3> t = findAABB(*all_loaded[i]->getMesh(), all_loaded[i]->actor->transform);

				//Print(all_loaded[i]->actor->transform);
				// check intersections to optimize what to compare later
				AABB the = { t.first, t.second};
				aabbs[i] = the;
			}

			std::vector<std::pair<size_t, size_t>> in = aabbPlaneSweep(aabbs);	
			for (std::pair<size_t, size_t>& a : in)
			{
				//std::cout << "(" << a.first << ", " << a.second << ")" << std::endl;
				std::shared_ptr<GraphicNode> ith = all_loaded[a.first];
				std::shared_ptr<GraphicNode> jth = all_loaded[a.second];

				std::vector<V3>& i_vertices = ith->getMesh()->positions;
				std::vector<V3>& i_normals = ith->getMesh()->normals;

				apply_worldspace(i_vertices, ith->actor->transform);
				apply_worldspace(i_normals, ith->actor->transform);

				std::vector<Face> i_faces;
				{
					Face currFace;
					std::vector<Vertex>& i_verts = ith->getMesh()->vertices;
					for (size_t i = 0; i < i_verts.size(); i++)
					{
						if (currFace.normal == i_verts[i].normal)
						{
							currFace.vertices.push_back(i_verts[i].pos);
						}
						else if (currFace.vertices.empty())
						{
							currFace.vertices.push_back(i_verts[i].pos);
							currFace.normal = i_verts[i].normal;
						}
						else
						{
							i_faces.push_back(currFace);
							currFace = Face();
						}
					}
				}
				V3 j_cm = findAverage(i_vertices);

				std::vector<V3>& j_vertices = jth->getMesh()->positions;
				std::vector<V3>& j_normals = jth->getMesh()->normals;

				apply_worldspace(j_vertices, jth->actor->transform);
				apply_worldspace(j_normals, jth->actor->transform);

				std::vector<Face> j_faces;
				{
					Face currFace;
					std::vector<Vertex>& j_verts = ith->getMesh()->vertices;

					for (size_t j = 0; j < j_verts.size(); j++)
					{
						if (currFace.normal == j_verts[j].normal)
						{
							currFace.vertices.push_back(j_verts[j].pos);
						}
						else if (currFace.vertices.empty())
						{
							currFace.vertices.push_back(j_verts[j].pos);
							currFace.normal = j_verts[j].normal;
						}
						else
						{
							j_faces.push_back(currFace);
							currFace = Face();
						}
					}
				}

				V3 i_cm = findAverage(j_vertices);

				CollisionInfo& info = sat(i_faces, j_faces);

				if(info.isColliding)
				{
					const float& m1 = ith->actor->mass;
					const float& m2 = jth->actor->mass;
					const V4 r1 = info.polytope - i_cm;
					const V4 r2 = info.polytope - j_cm;

					const float& e1 = ith->actor->elasticity;
					const float& e2 = jth->actor->elasticity;
					//if (info.isColliding)
					//{
					//	std::cout << ("e1 %f e2 %f dt %f", &e1, &e2, dt * 0.001f) << std::endl;
					//	ith->actor->transform.data[1][3] += ith->getMesh()->min[1] * e1 * dt * 0.001f;
					//	jth->actor->transform.data[1][3] += jth->getMesh()->min[1] * e2 * dt * 0.001f;
					//}
					//else continue;

					V4& u1 = ith->actor->linearVelocity;
					V4& u2 = jth->actor->linearVelocity;
					
					M4& rot1 = ith->actor->rotation;
					M4& rot2 = jth->actor->rotation;
					float& o1 = ith->actor->orie;
					float& o2 = jth->actor->orie;
					float& w1 = ith->actor->angleVel;
					float& w2 = jth->actor->angleVel;


					assert(m1 > 0.f);
					assert(m2 > 0.f);
					assert(0.f <= e1 && e1 <= 1.f);
					assert(0.f <= e2 && e2 <= 1.f);

					//V4 relativeVelocity = u2 - u1;

					//V4 collisionNormal = Normalize(r2 * 1.f);
					//float impulseScalar = -(1 + e1) * Dot(relativeVelocity, collisionNormal) / Dot(collisionNormal, collisionNormal * (1 / m1 + 1 / m2));
					//V4 impulse = impulseScalar * collisionNormal;
					//const V4 v1 = u1 + impulse * (1 / m1);

					//const V4 v2 = u2 - impulse * (1 / m2);


					const V4 v1 = e1 * (((m1-m2)/(m1+m2))*u1+((2*m2*u2) * (1 / (m1+m2))));
					const V4 v2 = e2 * (((m2-m1)/(m1+m2))*u2+((2*m1*u1) * (1 / (m1+m2))));
					
					V4 axis1 = Cross(r1, v1);
					V4 axis2 = Cross(r2, v2);
					w1 = Length(axis1) / (m1 * Length(r1)) * e1;
					w2 = Length(axis2) / (m2 * Length(r2)) * e2;

					//std::cin.get();
					o1 += w1;
					o2 += w2;

					if (ith->actor->isDynamic)
					{
						rot1 = Rotation(axis1, o1);
						o1 = o1 * .95f;
						
						const V4 res = reflect(v1, info.norm1);
						const V3 kl = { res.x, res.y, res.z };
						//ith->actor->apply_force(kl * 0.001f, dt);

						u1 = reflect(v1, info.norm1);
					}
					if (jth->actor->isDynamic)
					{
						rot2 = Rotation(axis2, o2);
						o2 = o2 * .95f;
						const V4 res = reflect(v2 * -1.f, info.norm2);
						const V3 kl = { res.x, res.y, res.z };
						std::cout << o2 << std::endl;
						//jth->actor->apply_force(kl * 0.001f, dt);
						u2 = reflect(v2 * -1.f, info.norm2);
					}
				}
			}

			for (const auto node : all_loaded)
			{
				const float& m = node->actor->mass;
				if (node->actor->isDynamic)
				{
					//node->actor->apply_force(m * GRAVITY * 0.0001f, dt);
				}
			}

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (std::shared_ptr<GraphicNode> a : all_loaded)
			{
				const V4 color(1, 1, 1, 1);

				a->getShader()->setM4(cam.pv(), "m4ProjViewPos");

				std::shared_ptr<ShaderResource> script = a->getShader();


				a->actor->update(dt * 0.4f);
				M4& wst = a->actor->transform;
				if (showDebugRender)
				{
					if (isPressed)
					{
						std::cout << std::endl;
					}

					MeshResource& m = *a->getMesh();
					Debug::DrawBB(m, V4(0, 1, 1, 1), wst);
					
					std::pair<V3, V3> aabb = findAABB(m, wst);
					Debug::DrawAABB(aabb, V4(1, 0, 0, 1));
				}
				light.bindLight(script, cam.getPos());
				a->DrawScene(cam.pv() * wst, color);
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
			dt = std::chrono::duration_cast<ms>(stop - start).count();
		}
	}

	void ExampleApp::RenderUI()
	{
		bool show = true;
		ImGui::Begin("Panel", &show, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Checkbox("Debug Mode: ", &showDebugRender);
		ImGui::Text("frames: %d %.0f", frameIndex);
		ImGui::End();
	}

} // namespace Example
