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

	const M4 quaternionToRotationMatrix(const Quaternion& q)
	{
		M4 rotationMatrix;

		float w = q.getW();
		float x = q.getX();
		float y = q.getY();
		float z = q.getZ();

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

	const const V3 ray_intersection(
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
		w = a = s = d = q = e = false;
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

			case GLFW_KEY_Q: q = action; break;
			case GLFW_KEY_E: e = action; break;
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

			// TextureResource
			fireHydrantTexture = std::make_shared<TextureResource>("textures/cubepic.png");
			fireHydrantTexture->LoadFromFile();

			// shaderResource
			fireHydrantScript = std::make_shared<ShaderResource>();
			fireHydrantScript->LoadShader(fireHydrantScript->vs, fireHydrantScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// Actor
			Actor *fireHydrantActor = new Actor();

			// GraphicNode
			fireHydrant = std::make_shared<GraphicNode>(fireHydrantMesh, fireHydrantTexture, fireHydrantScript, fireHydrantActor);

			all_loaded.push_back(fireHydrant);

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

			// shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->LoadShader(cubeScript->vs, cubeScript->ps, "textures/vs.glsl", "textures/psNoTexture.glsl");

			// Actor
			Actor *cubeActor = new Actor();

			// GraphicNode
			cube = std::make_shared<GraphicNode>(cubeMesh, cubeTexture, cubeScript, cubeActor);

			all_loaded.push_back(cube);

			// MeshResource
			//std::vector<unsigned> quadIndices;
			//std::vector<V3> quadCoords;
			//std::vector<V2> quadTexels;
			//std::vector<V3> quadNormals;
			//std::vector<Vertex> quadVertices;
			//quadMesh = MeshResource::LoadObj("textures/quad.obj", quadIndices, quadCoords, quadTexels, quadNormals, quadVertices);
			//quadMesh->indicesAmount = quadIndices;
			//quadMesh->positions = quadCoords;
			//quadMesh->texels = quadTexels;
			//quadMesh->normals = quadNormals;
			//quadMesh->vertices = quadVertices;
			//quadMesh->min = quadMesh->find_bounds().first;
			//quadMesh->max = quadMesh->find_bounds().second;

			//// Actor
			//Actor *quadActor = new Actor();

			//// GraphicNode
			//quad = std::make_shared<GraphicNode>(quadMesh, cubeTexture, cubeScript, quadActor);

			//all_loaded.push_back(quad);

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
		return hit;
		std::cout << "x: " << hit[0] << ", y: " << hit[1] << ", z: " << hit[2] << std::endl;
	}

	void
	ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// deltatime
		float dt = 0.01;
		Ray ray(V3(FLT_MAX, FLT_MAX, FLT_MAX), V3(FLT_MAX, FLT_MAX, FLT_MAX));

		// gravity
		const float g = -9.806e-3f;
#define GRAVITY V3(0, g, 0)

		Camera cam(90, (float)width / height, 0.01f, 1000.0f);
		cam.setPos(V4(0, 1, 3));
		cam.setRot(V4(0, 1, 0), M_PI);

		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);

		float camSpeed = .08f;

		// set identies
		fireHydrant->actor->transform = Translate(V4(3, 0, 0));
		
		cube->actor->transform = Translate(V4(-3, 0, 0));

		cube->actor->linearVelocity = V3(.0005f, 0, 0);
		//fireHydrant->actor->linearVelocity = V3(-.0005f, 0, 0);

		while (this->window->IsOpen())
		{
			//--------------------ImGui section--------------------

			auto start = std::chrono::high_resolution_clock::now();

			//--------------------math section--------------------
			cam.setPos(cam.getPos() + Normalize(V4((d - a), (q - e), (w - s))) * -camSpeed);
			V3 rayOrigin = cam.getPos() * 1.f;
			
			// effect of gravity
			//fireHydrant->actor->apply_force(GRAVITY, dt);

			//fireHydrant world space
			fireHydrant->actor->transform = Rotation(V4(40, 0, 1), -0.012f) * Rotation(V4(0, 1, 0), 0.004f) * fireHydrant->actor->transform
			
			//* Translate(fireHydrant->actor->velocity)
				;

			//fireHydrant view space
			fireHydrantProjectionViewTransform = cam.pv() * fireHydrant->actor->transform;// * Scalar(V4(.1, .1, .1));

			// cube world space
			/*quad->actor->transform = quad->actor->transform
										* Translate(V4(0, 0, cos(frameIndex / 20.f)))
				;*/

			// cube view space
			cubeProjectionViewTransform = cam.pv() * cube->actor->transform;

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
					// make sure w is one when multiplying V4 and float (fixed in mathLib.h)
					Debug::DrawLine(V4(resultingHit - V3(0, 3, 0), 1), V4(resultingHit - V3(0, 0, 0), 1), V4(1, 0, 0, 1));
					//Debug::DrawSquare(V4(resultingHit, 1));
				}
				resultingHit = ray_intersection(ray, fireHydrantWorldSpaceTransform, fireHydrantMesh->positions, fireHydrantMesh->indicesAmount, &(fireHydrantMesh)->normals);
			}
			//cube->actor->transform = Translate(V4(resultingHit, 1));

			//for (std::shared_ptr<GraphicNode>& a : all_loaded)
			//{
			//	apply_worldspace(a->getMesh()->positions, a->actor->transform);
			//	a->getMesh()->find_bounds();
			//	V3 gg = a->actor->transform.toV3();
			//	printf("%f, %f, %f\n", gg.x, gg.y, gg.z);
			//	// check intersections to optimize what to compare later
			//	AABB the = { a->getMesh()->min, a->getMesh()->max };
			//}

			//std::vector<std::pair<size_t, size_t>> in = aabbPlaneSweep(aabbs);
			for (size_t i = 0; i < 1; i++)
			{
				std::shared_ptr<GraphicNode>& ith = cube;//all_loaded[0];
				std::shared_ptr<GraphicNode>& jth = fireHydrant;//all_loaded[1];
				
				std::vector<V3> i_vertices = ith->getMesh()->positions;
				apply_worldspace(i_vertices, ith->actor->transform);
				
				std::vector<V3> j_vertices = jth->getMesh()->positions;
				apply_worldspace(j_vertices, jth->actor->transform);
				std::vector<V3> simplex_placeholder;

				if (gjk(simplex_placeholder, i_vertices, j_vertices))
				{
					for (size_t i = 0; i < simplex_placeholder.size(); ++i)
					{
						V4 line1 = V4(V3(simplex_placeholder[i]), 1);
						V4 line2 = V4(V3(simplex_placeholder[(i + 1) % simplex_placeholder.size()]), 1);
						Debug::DrawLine(line1, line2, V4(1, 1, 1, 1));
					}
					
					//std::cout << "collision detected! frame: " << frameIndex << std::endl;
					V3 normal;
					float depth;
					std::vector<V3> suppe = epa(normal, depth, simplex_placeholder,
						i_vertices, j_vertices);
					V3 p = get_collision_point_in_model_space(suppe, normal, depth);
					Debug::DrawLine(V4(p, 1), V4(p + normal * depth, 1));
					
					//std::cout << "normal: " << normal[0] << normal[1] << normal[2] << "\t";
					//std::cout << "point: " << p[0] << p[1] << p[2] << "\t";
					std::cout << "depth: " << depth << std::endl;
					// handle COllision responses here
					//e * (ith->actor->linearVelocity * ith->actor->mass * .8f + jth->actor->linearVelocity * jth->actor->mass) = ;

					// temporary display of the collision working
					ith->actor->linearVelocity = ith->actor->linearVelocity * -1.f;
					jth->actor->linearVelocity = jth->actor->linearVelocity * -1.f;


					//fireHydrant->actor->apply_linear_impulse(ray, (fireHydrantProjectionViewTransform * V4(fireHydrantMesh->center_of_mass, 1)).toV3(), resultingHit);
				}
				// TODO create an impulse

			}

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			fireHydrant->getShader()->setM4(cam.pv(), "m4ProjViewPos");
			cube->getShader()->setM4(cam.pv(), "m4ProjViewPos");

			std::shared_ptr<ShaderResource>& script = all_loaded[0]->getShader();
			V4& color = fireHydrantColor;

			for (std::shared_ptr<GraphicNode>& a : all_loaded)
			{
				a->actor->update(dt);
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
				//light.bindLight(script, cam.getPos());
				//a->DrawScene(cam.pv() * wst, color);
			}
			if (showDebugRender)
			{
				Debug::DrawLine(V4(1, 1, 1, 1), V4(1, 1, 1, 1), V4(1, 1, 1, 1));
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
