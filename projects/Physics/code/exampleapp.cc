//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
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

	const V3 slow_intersection(
		Ray& r,
		M4 WorldSpaceTransform,
		std::vector<V3>& i_worldSpace_coords,
		std::vector<unsigned>& i_meshModel_indices,
		std::vector<V3>* normals = nullptr)
	{
		V3 closest_point = NAN_V3;

		//use the world space coordinates for the entire model
		for (size_t i = 0; i < i_meshModel_indices.size(); i += 3)
		{
			if (i_worldSpace_coords.size() - 1 < i_meshModel_indices[i + 2]) break;
			Plane p = Plane(NAN_V3, NAN_V3);
			//V3 a = i_worldSpace_coords[i_meshModel_indices[i + 0]];
			//V3 b = i_worldSpace_coords[i_meshModel_indices[i + 1]];
			//V3 c = i_worldSpace_coords[i_meshModel_indices[i + 2]];

			V3 a = (Transpose(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 0]], 1)).toV3();
			V3 b = (Transpose(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 1]], 1)).toV3();
			V3 c = (Transpose(WorldSpaceTransform) * V4(i_worldSpace_coords[i_meshModel_indices[i + 2]], 1)).toV3();

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
			V3 currentIntersect = r.intersect(p);
			if (currentIntersect == NAN_V3) continue;

			//check if it's within the triangle's edges
			V3 x1 = Cross(b - a, currentIntersect - a);
			V3 x2 = Cross(c - a, currentIntersect - b);
			V3 x3 = Cross(a - c, currentIntersect - c);
			if (Dot(x1, x2) > 0.f && Dot(x2, x3) > 0.f && Dot(x3, x1) > 0.f)
			{
				if (closest_point != NAN_V3)
				{
					V3 curr_dist = currentIntersect - r.origin;
					V3 best_dist = closest_point - r.origin;

					// only update if it is the closest point on the mesh.
					if (Length2(curr_dist) < Length2(best_dist))
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
			fireHydrantMesh = MeshResource::LoadObj("textures/fireHydrant.obj", fireIndices, fireCoords, fireTexels, fireNormals, fireVertices);
			fireHydrantMesh->indicesAmount = fireIndices;
			fireHydrantMesh->positions = fireCoords;
			fireHydrantMesh->texels = fireTexels;
			fireHydrantMesh->normals = fireNormals;
			fireHydrantMesh->vertices = fireVertices;
			// fireHydrantMesh->positions = MeshResource::LoadVerticesFromFile("textures/fireHydrant.obj");
			assert(fireCoords.size() == fireHydrantMesh->positions.size());

			// TextureResource
			fireHydrantTexture = std::make_shared<TextureResource>("textures/cubepic.png");
			fireHydrantTexture->LoadFromFile();

			// shaderResource
			fireHydrantScript = std::make_shared<ShaderResource>();
			fireHydrantScript->LoadShader(fireHydrantScript->vs, fireHydrantScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// Actor
			Actor temp;
			Actor *fireHydrantActor = &temp;

			// GraphicNode
			fireHydrant = std::make_shared<GraphicNode>(fireHydrantMesh, fireHydrantTexture, fireHydrantScript, fireHydrantActor);

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
			// cubeMesh->positions = MeshResource::LoadVerticesFromFile("textures/cube.obj");
			
			cubeTexture = std::make_shared<TextureResource>("textures/red.png");

			// shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->LoadShader(cubeScript->vs, cubeScript->ps, "textures/vs.glsl", "textures/psNoTexture.glsl");
			// note: bindTexture() still requires a texture, but just won't use it

			// Actor
			Actor temp2;
			Actor *cubeActor = &temp2;

			// GraphicNode
			cube = std::make_shared<GraphicNode>(cubeMesh, cubeTexture, cubeScript, cubeActor);

			// MeshResource
			std::vector<unsigned> quadIndices;
			std::vector<V3> quadCoords;
			std::vector<V2> quadTexels;
			std::vector<V3> quadNormals;
			std::vector<Vertex> quadVertices;
			quadMesh = MeshResource::LoadObj("textures/quad.obj", quadIndices, quadCoords, quadTexels, quadNormals, quadVertices);
			quadMesh->indicesAmount = quadIndices;
			quadMesh->positions = quadCoords;
			quadMesh->texels = quadTexels;
			quadMesh->normals = quadNormals;
			quadMesh->vertices = quadVertices;
			// quadMesh->positions = MeshResource::LoadVerticesFromFile("textures/quad.obj");
			
			// Actor
			Actor temp3;
			Actor *quadActor = &temp3;

			// GraphicNode
			quad = std::make_shared<GraphicNode>(quadMesh, cubeTexture, cubeScript, quadActor);

			this->window->SetUiRender([this]()
									  { this->RenderUI(); });
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	 */
	
	void fuck_test()
	{
		V3 rayOrigin = V3(0, 0, 0);
		V3 rayDir = V3(0, 0, 1);

		Ray ray(rayOrigin, rayDir);

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

		vertexPositions.push_back(bottomLeft);
		vertexPositions.push_back(topMiddle);
		vertexPositions.push_back(bottomRight);

		std::vector<uint32> indices;

		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(0);

		std::vector<V3>* normals = nullptr;

		V3 hit = slow_intersection(ray, emptyMatrix, vertexPositions, indices, normals); // resulting hit should be 0, 0, 1
		std::cout << "x: " << hit.x << ", y: " << hit.y << ", z: " << hit.z << std::endl;
	}

	void
	ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);


		// gravity
		const float g = -9.806e-3f;
		std::cout << (bool(NAN) == bool(NAN)) << std::endl;
		std::cout << (bool(nanf("")) == bool(nanf(""))) << std::endl;
		std::cout << (bool(NAN) != bool(NAN)) << std::endl;
		std::cout << (bool(nanf("")) != bool(nanf("")));
		fuck_test();
		Camera cam(90, (float)width / height, 0.01f, 1000.0f);
		// cam.setPos(V4(0, 4, 3));
		cam.setRot(V4(0, 1, 0), M_PI);

		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);

		float camSpeed = .8f;

		// set identies
		fireHydrantWorldSpaceTransform = Translate(V4(0, 0, 15));
		fireHydrantProjectionViewTransform = Translate(V4());
		fireHydrantMesh->findbounds();
		

		cubeWorldSpaceTransform = Translate(V4(0, 0, -5));
		cubeProjectionViewTransform = Translate(V4());

		M4 quadWorldSpaceTransform[100];
		M4 quadProjectionViewTransform[100];
		for (size_t i = 0; i < 10; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{
				quadProjectionViewTransform[i * 10 + j] = Translate(V4());
				quadWorldSpaceTransform[i * 10 + j] = Translate(V4(i * 2, j * 2, 0));
			}			
		}

		//printf("%f %f %f\n", x, y, z);
		


		plane = new Plane(V3(0, 0, -0), V3(0, 0, 1));
		while (this->window->IsOpen())
		{
			//--------------------ImGui section--------------------

			auto start = std::chrono::high_resolution_clock::now();

			//--------------------math section--------------------
			cam.setPos(cam.getPos() + Normalize(V4((d - a), (q - e), (w - s))) * -camSpeed);
			V3 rayOrigin = cam.getPos() * 1.f;

			// std::cout << "frame " << frameIndex << std::endl;
			
			Debug::DrawBB(*fireHydrant->getMesh(), V4(0, 1, 1, 1), fireHydrantWorldSpaceTransform);
			Debug::DrawAABB(*fireHydrant->getMesh(), V4(1, 0, 0, 1), fireHydrantWorldSpaceTransform);
			//Print(fireHydrantWorldSpaceTransform);
			//Implement a gravitational acceleration on the fireHydrant
			fireHydrant->actor->velocity = fireHydrant->actor->velocity + fireHydrant->actor->mass * g;

			//fireHydrant world space
			//fireHydrantWorldSpaceTransform = /*Rotation(V4(0, 0, 1), -0.012f) * Rotation(V4(0, 1, 0), 0.004f) */ fireHydrantWorldSpaceTransform
			
			// effect of gravity
			/* *
			Translate(V4(0, -1, 0) * fireHydrant->actor->velocity)*/;

			//fireHydrant view space
			fireHydrantProjectionViewTransform = cam.pv() * fireHydrantWorldSpaceTransform/* * Scalar(V4(.1, .1, .1))*/;

			// cube world space
			cubeWorldSpaceTransform = cubeWorldSpaceTransform
										//* Translate(V4(0, 0, cos(frameIndex / 20.f)))
				;

			// // cube view space
			cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;

			/*for (size_t i = 0; i < countLines; i++)
			{
				Debug::DrawLine(start[i], dirSize[i], 1.f, V4(1, 1, 1, 1));
			}*/

			Debug::DrawBB(*fireHydrant->getMesh(), V4(0, 1, 1, 1), fireHydrantWorldSpaceTransform);
			Debug::DrawAABB(*fireHydrant->getMesh(), V4(1, 0, 0, 1), fireHydrantWorldSpaceTransform);

			Plane left_plane(V3(fireHydrantMesh->left, 0, 0), V3(fireHydrantMesh->left, 0, 0));
			Plane right_plane(V3(fireHydrantMesh->right, 0, 0), V3(fireHydrantMesh->right, 0, 0));

			Plane top_plane(V3(0, fireHydrantMesh->top, 0), V3(0, fireHydrantMesh->top, 0));
			Plane bottom_plane(V3(0, fireHydrantMesh->bottom, 0), V3(0, fireHydrantMesh->bottom, 0));

			Plane front_plane(V3(0, 0, fireHydrantMesh->front), V3(0, 0, fireHydrantMesh->front));
			Plane back_plane(V3(0, 0, fireHydrantMesh->back), V3(0, 0, fireHydrantMesh->back));

			if (isPressed)
			{
				glfwGetCursorPos(this->window->GetHandle(), &mouseDirX, &mouseDirY);
				// shot a ray
				V4 normalizedDeviceCoordinates(mouseDirX / width * 2 - 1, 1 - mouseDirY / height * 2, 1, 1);
				V4 mousePickingWorldSpace = Inverse(cam.pv()) * normalizedDeviceCoordinates;
				Ray ray(rayOrigin, mousePickingWorldSpace.toV3() - rayOrigin);

				V3 res_left = ray.intersect(left_plane);
				V3 res_two = ray.intersect(right_plane);

				V3 res_top = ray.intersect(top_plane);
				V3 res_bottom = ray.intersect(bottom_plane);

				V3 res_front = ray.intersect(front_plane);
				V3 res_back = ray.intersect(back_plane);


				std::vector<V3> tt;
				tt.push_back(res_left);
				tt.push_back(res_two);

				tt.push_back(res_top);
				tt.push_back(res_bottom);

				tt.push_back(res_front);
				tt.push_back(res_back);

				resultingHit = ray.minDist(tt);
				if (resultingHit != NAN_V3)
				{
					Debug::DrawLine(V4(resultingHit - V3(0, 3, 0), 1), V4(resultingHit - V3(0, 0, 0), 1), V4(1, 0, 0, 1));
					Debug::DrawSquare(V4(resultingHit, 1));
				}
				resultingHit = slow_intersection(ray, fireHydrantWorldSpaceTransform, fireHydrantMesh->positions, fireHydrantMesh->indicesAmount, &(fireHydrantMesh)->normals);
			}

			cubeWorldSpaceTransform = Translate(V4(resultingHit.x, resultingHit.y, resultingHit.z, 1));

			for (size_t i = 0; i < 100; i++)
			{
				quadProjectionViewTransform[i] = cam.pv() * quadWorldSpaceTransform[i];
			}

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			fireHydrantScript->setM4(cam.pv(), "m4ProjViewPos");
			cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			light.bindLight(fireHydrantScript, cam.getPos());
			fireHydrant->DrawScene(fireHydrantProjectionViewTransform, fireHydrantColor); // cause

			light.bindLight(cubeScript, cam.getPos());
			cube->DrawScene(cubeProjectionViewTransform, cubeColor); // cause

			for (int i = 0; i < 100; i++)
			{
				if (true || plane->pointIsOnPlane(quadWorldSpaceTransform[i].toV3(), .0000001))
				{
					//cube->DrawScene(quadProjectionViewTransform[i], fireHydrantColor);
				}
			}

			// usleep(10000);
			this->window->Update();
			frameIndex++;

			Debug::Render(cam.pv());
			this->window->SwapBuffers();
			auto finish = std::chrono::system_clock::now();
#ifdef __linux__
			duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
#endif
		}
	}

	void ExampleApp::RenderUI()
	{
		bool show = true;
		ImGui::Begin("Mega Cringe", &show, ImGuiWindowFlags_NoSavedSettings);
		float cube[3];
		for (int i = 0; i < 3; i++)
		{
			cube[i] = cubeWorldSpaceTransform[i][3];
		}
		ImGui::Text("cube: %.3f\t%.3f\t%.3f", cube[0], cube[1], cube[2]);

		ImGui::SliderFloat("x", &x, -5.f, 5.f);
		ImGui::SliderFloat("y", &y, -5.f, 5.f);
		ImGui::SliderFloat("z", &z, -5.f, 5.f);

		// plane->normal = V3(x, y, z);
		ImGui::Text("resultingHit: %.3f\t%.3f\t%.3f", resultingHit.x, resultingHit.y, resultingHit.z);

		ImGui::Text("frames: %d %.0f", frameIndex, 1e6f / float(duration));

		ImGui::End();
	}

} // namespace Example
