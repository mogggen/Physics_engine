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

	const M4 quaternionToRotationMatrix(const Quaternion& shift)
	{
		M4 rotationMatrix;

		float w = shift.getW();
		float x = shift.getX();
		float y = shift.getY();
		float z = shift.getZ();

		float ww = w * w;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;
		float xx = x * x;
		float xy = x * y;
		float xz = x * z;
		float yy = y * y;
		float yz = y * z;
		float zz = z * z;

		rotationMatrix[0][0] = 1 - 2 * (yy + zz);
		rotationMatrix[0][1] = 2 * (xy - wz);
		rotationMatrix[0][2] = 2 * (xz + wy);
		rotationMatrix[0][3] = 0.0;

		rotationMatrix[1][0] = 2 * (xy + wz);
		rotationMatrix[1][1] = 1 - 2 * (xx + zz);
		rotationMatrix[1][2] = 2 * (yz - wx);
		rotationMatrix[1][3] = 0.0;

		rotationMatrix[2][0] = 2 * (xz - wy);
		rotationMatrix[2][1] = 2 * (yz + wx);
		rotationMatrix[2][2] = 1 - 2 * (xx + yy);
		rotationMatrix[2][3] = 0.0;

		rotationMatrix[3][0] = 0.0;
		rotationMatrix[3][1] = 0.0;
		rotationMatrix[3][2] = 0.0;
		rotationMatrix[3][3] = 1.0;

		return rotationMatrix;
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
			
			if (point_in_triangle_3D(currentIntersect, a, b, c))
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

	// Function to calculate the projection of an object onto a given axis
	float ProjectOntoAxis(const std::vector<Vertex>& vertices, const V3& axis) {
		float min = FLT_MAX;
		float max = -FLT_MAX;

		for (const Vertex& vertex : vertices) {
			float projection = Dot(vertex.pos, axis);
			if (projection < min) {
				min = projection;
			}
			if (projection > max) {
				max = projection;
			}
		}

		return max - min;
	}

	// Function to calculate the penetration point along the axis
	V3 CalculatePenetrationPoint(const std::vector<Vertex>& vertices, const V3& axis) {
		V3 penetrationPoint(0.0f, 0.0f, 0.0f);
		float maxProjection = -FLT_MAX;

		for (const Vertex& vertex : vertices) {
			float projection = Dot(vertex.pos, axis);
			if (projection > maxProjection) {
				maxProjection = projection;
				penetrationPoint = vertex.pos;
			}
		}

		return penetrationPoint;
	}

	struct CollisionInfo {
		bool isColliding;
		float depth;
		V3 pen1;
		V3 pen2;
	};

	CollisionInfo sat(const std::vector<Vertex>& i_vertices, const std::vector<Vertex>& j_vertices) {
		CollisionInfo collisionInfo;
		collisionInfo.isColliding = true;
		collisionInfo.depth = FLT_MAX;

		//V3 previous;

		for (size_t i = 0; i < i_vertices.size(); i++) {
			for (size_t j = 0; j < j_vertices.size(); j++) {
				
				V3 axis = Cross(i_vertices[i].normal, j_vertices[j].normal);

				//// avoid redoing work
				//if (!Dot(axis, previous)) continue;
				//previous = axis;
				//
				// Check for separation along the axis
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
					collisionInfo.pen1 = CalculatePenetrationPoint(i_vertices, axis);
					collisionInfo.pen2 = CalculatePenetrationPoint(j_vertices, axis);
				}
			}
		}

		return collisionInfo; // Collision detected
	}

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
			assert(fireCoords.size() == fireHydrantMesh->positions.size());
			fireHydrantMesh->min = fireHydrantMesh->find_bounds().first;
			fireHydrantMesh->max = fireHydrantMesh->find_bounds().second;
			//fireHydrantMesh->center_of_mass = fireHydrantMesh->findCenterOfMass(fireHydrantMesh->positions);

			// TextureResource
			fireHydrantTexture = std::make_shared<TextureResource>("textures/cubepic.png");
			fireHydrantTexture->LoadFromFile();

			// shaderResource
			fireHydrantScript = std::make_shared<ShaderResource>();
			fireHydrantScript->LoadShader(fireHydrantScript->vs, fireHydrantScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// Actor
			Actor *fireHydrantActor = new Actor();
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
			cubeMesh = MeshResource::LoadObj("textures/cube.obj", cubeIndices, cubeCoords, cubeTexels, cubeNormals, cubeVertices);
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
			Actor *cubeActor = new Actor();
			cubeActor->mass = 300;
			cubeActor->elasticity = 0.7;

			// GraphicNode
			cube = std::make_shared<GraphicNode>(cubeMesh, cubeTexture, cubeScript, cubeActor);

			all_loaded.push_back(cube);

			this->window->SetUiRender([this]()
									  { this->RenderUI(); });
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	 */

	V3 mesh_intersection_test(Ray& ray)
	{
		V4 identity[4] = {
			V4(1, 0, 0, 0),
			V4(0, 1, 0, 0),
			V4(0, 0, 1, 0),
			V4(0, 0, 0, 1)
		};

		M4 emptyMatrix = M4(identity);

		//Print(emptyMatrix);

		std::vector<V3> vertexPositions;

		V3 bottomLeft = V3(-1, -1, 5);
		V3 topMiddle = V3(0, 1, 5);
		V3 bottomRight = V3(1, -1, 5);
		
		//V3 bottomLeft = V3();
		//TODO: CONTINUE CODING HERE, ADD ANOTHER FACE TO MAKE SURE DEPTH TEST WORKS
		vertexPositions.push_back(bottomLeft);
		vertexPositions.push_back(topMiddle);
		vertexPositions.push_back(bottomRight);

		std::vector<uint32> indices;

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		std::vector<V3>* normals = nullptr;

		V3 hit = ray_intersection(ray, emptyMatrix, vertexPositions, indices, normals); // resulting hit should be 0, 0, 1
		//std::cout << "x: " << hit[0] << ", y: " << hit[1] << ", z: " << hit[2] << std::endl;
		return hit;
	}

	void gjk_collision_test(Actor a, Actor b)
	{
		std::vector<V3> Coords;
		std::vector<unsigned> Indices;
		std::vector<Vertex> faceBuffer;
		{
			std::vector<V2> Texels;
			std::vector<V3> Normals;
			/*std::shared_ptr<MeshResource> PyramidMesh = */MeshResource::LoadObj("textures/pyramid.obj", Indices, Coords, Texels, Normals, faceBuffer);
		}

		//v -1.000000 1.000000 -1.000000
		//v 1.000000 -1.000000 -1.000000
		//v -1.000000 -1.000000 -1.000000
		//v -1.000000 -1.000000 1.000000
		//vn 0.5774 0.5774 0.5774
		//vn -1.0000 -0.0000 -0.0000
		//vn -0.0000 -1.0000 -0.0000
		//vn -0.0000 -0.0000 -1.0000
		//vt 0.625000 0.250000
		//vt 0.375000 0.000000
		//vt 0.375000 0.500000
		//vt 0.375000 0.250000
		//vt 0.625000 0.500000
		//f 1/1/1 4/2/1 2/3/1
		//f 4/2/2 1/1/2 3/4/2
		//f 3/4/3 2/3/3 4/2/3
		//f 3/4/4 1/5/4 2/3/4

		std::vector<V3> resultingSimplex; 

		std::vector<V3> leftPos;
		std::vector<V3> rightPos;


		Print(a.transform);
		Print(b.transform);
		for (const V3& c : Coords)
		{
			const V3 left = (a.transform * V4(c, 1)).toV3();
			leftPos.push_back(left);
			
			const V3 right = (b.transform * V4(c, 1)).toV3();
			rightPos.push_back(right);
			//left.normal = Normalize((a.transform * V4(left.normal)).toV3());
			//leftNorm.push_back(v.normal);
		}

		if (gjk(resultingSimplex, leftPos, rightPos))
		{
			std::cout << "collision" << std::endl;
			std::cout << "Simplex size: " << resultingSimplex.size() << std::endl;
			
			float pen = 0.f;
			V3 res_norm;
			std::vector<Vertex> leftVert;
			std::vector<Vertex> rightVert;
		
			for (Vertex& v : faceBuffer)
			{
				leftVert.insert(leftVert.begin(), Vertex
					{
						(a.transform * V4(v.pos, 1)).toV3(),
						V4(1, 1, 1, 1),
						v.texel,
						Normalize((a.transform * V4(v.normal)).toV3())
					});
				rightVert.insert(rightVert.begin(), Vertex
					{
						(b.transform * V4(v.pos, 1)).toV3(),
						V4(1, 1, 1, 1),
						v.texel,
						Normalize((b.transform * V4(v.normal)).toV3())
					});
			}
			
			// use faces here
			//if (epa(res_norm, pen, resultingSimplex, Indices, leftPos, rightPos).size())
			{

			}
		}
		else
		{
			std::cout << "No collision" << std::endl;
		}

		//std::cout << "x: " << hit[0] << ", y: " << hit[1] << ", z: " << hit[2] << std::endl;
		return;

	}

	void
	ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		time_t f = time(nullptr);
		std::cout << "seed: " << f << std::endl;
		srand(f);

		for (size_t i = 1; i < 4; i++)
		{
			const GraphicNode bro = *texturedCube.get();
			all_loaded.push_back(std::make_shared<GraphicNode>(bro));
			all_loaded[i]->actor = new Actor();
			all_loaded[i]->actor->mass = 2;
			all_loaded[i]->actor->elasticity = 0.9;
			all_loaded[i]->actor->linearVelocity = V4(0, 0, 0, 0);
			float x_rand = 0;//rand() / (float)RAND_MAX * 20.f - 10.f;
			float z_rand = 0;//rand() / (float)RAND_MAX * 20.f - 10.f;
			all_loaded[i]->actor->transform = Translate(V4(x_rand, 3.f * i, z_rand));
			//std::cout << all_loaded[i]->actor->transform.toV3().z << std::endl;
		}

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
		cube->actor->transform = Translate(V4(0, -10.5f, 0)) * Scalar(V4(10, 1, 10, 1));
		//texturedCube->actor->transform = Translate(V4(0, 1.5f, 0));
		
		cube->actor->linearVelocity = V3(0, 0, 0);
		texturedCube->actor->linearVelocity = V3(0, 0, 0);
		
		for	(auto g : all_loaded)
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
			for (size_t i = 1; i < all_loaded.size(); i++)
			{
				const float& m = all_loaded[i]->actor->mass;
				all_loaded[i]->actor->apply_force(m * GRAVITY * 0.0001f, dt);
			}

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
				std::cout << "(" << a.first << ", " << a.second << ")" << std::endl;
				std::shared_ptr<GraphicNode> ith = all_loaded[a.first];
				std::shared_ptr<GraphicNode> jth = all_loaded[a.second];
				
				
				V3& i_cm = ith->getMesh()->center_of_mass;
				std::vector<V3>& i_vertices = ith->getMesh()->positions;
				std::vector<V3>& i_normals = ith->getMesh()->normals;
				
				apply_worldspace(std::vector<V3>{ i_cm}, ith->actor->transform);
				apply_worldspace(i_vertices, ith->actor->transform);
				apply_worldspace(i_normals, ith->actor->transform);


				V3& j_cm = jth->getMesh()->center_of_mass;
				std::vector<V3>& j_vertices = jth->getMesh()->positions;
				std::vector<V3>& j_normals = jth->getMesh()->normals;

				apply_worldspace(std::vector<V3>{ j_cm}, jth->actor->transform);
				apply_worldspace(j_vertices, jth->actor->transform);
				apply_worldspace(j_normals, jth->actor->transform);


				CollisionInfo& info = sat(ith->getMesh()->vertices, jth->getMesh()->vertices);
				if (info.isColliding)
				{

					const float& m1 = ith->actor->mass;
					const float& m2 = jth->actor->mass;
					V4& u1 = ith->actor->linearVelocity;
					V4& u2 = jth->actor->linearVelocity;
					const float& e1 = ith->actor->elasticity;
					const float& e2 = jth->actor->elasticity;
					
					Quaternion q1 = ith->actor->angularVelocity;
					Quaternion q2 = jth->actor->angularVelocity;


					float& o1 = ith->actor->orie;
					float& o2 = jth->actor->orie;
					float& w1 = ith->actor->angleVel;
					float& w2 = jth->actor->angleVel;

					assert(m1);
					assert(m2);
					assert(0.f <= e1 && e1 <= 1.f);
					assert(0.f <= e2 && e2 <= 1.f);
					const V4 v1 = e1 * (((m1-m2)/(m1+m2))*u1+((2*m2*u2) * (1 / (m1+m2))));
					const V4 v2 = e2 * (((m2-m1)/(m1+m2))*u2+((2*m1*u1) * (1 / (m1+m2))));
					
					
					//angularMomentum
					V3 arm1 = info.pen1 - i_cm;
					V3 arm2 = info.pen2 - j_cm;

					printf("penetration point: %.8f\n", info.depth);
					printf("penetration point: %.8f %.8f %.8f\n", arm1.x, arm1.y, arm1.z);
					printf("penetration point: %.8f %.8f %.8f\n", arm2.x, arm2.y, arm2.z);

					//if (info.depth)
					{
						//M4& i_m4 = ith->actor->transform;
						//i_m4 = Translate(-info.penetrationDepth * Normalize(u1)) * i_m4;
						//M4& j_m4 = jth->actor->transform;
						//j_m4 = Translate(V4(j_m4.toV3(), 1) - 0.001f * Normalize(u2));
	
						//if (jth->actor->transform[1][3] > ith->actor->transform[1][3])
							//jth->actor->transform[1][3] -= u2.y;
						//else
						//	ith->actor->transform[1][3] += 0.010f;
					}

					u1 = v1;
					u2 = v2;
					
					// apply resting forces
					//ith->actor->apply_force(-e1 * m1 * GRAVITY * 0.00001f, dt);
					//jth->actor->apply_force(-e2 * m2 * GRAVITY * 0.00001f, dt);
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
