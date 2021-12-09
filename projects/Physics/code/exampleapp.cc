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
			fireHydrantMesh = MeshResource::LoadObj("textures/sphere.obj");
			fireHydrantMesh->positions = MeshResource::LoadVerticesFromFile("textures/sphere.obj");

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
			cubeMesh = MeshResource::LoadObj("textures/cube.obj");

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
			quadMesh = MeshResource::LoadObj("textures/quad.obj");

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

	void
	ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// gravity
		const float g = -9.806e-3f;

		Camera cam(90, (float)width / height, 0.01f, 1000.0f);
		cam.setPos(V4(0, 4, 3));
		cam.setRot(V4(0, 1, 0), M_PI);

		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);

		float camSpeed = .8f;

		// set identies
		fireHydrantWorldSpaceTransform = fireHydrantProjectionViewTransform = Translate(V4());

		fireHydrantMesh->findbounds();
		

		cubeWorldSpaceTransform = cubeProjectionViewTransform = Translate(V4());

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

			// std::cout << "frame " << frameIndex << std::endl;
			
			Debug::DrawLine(V4(V3(), 1), cam.getPos() + V3(mouseDirX - this->width / 2, mouseDirY - this->height / 2, 50.f), V4(1, 1, 1, 1));
			// Debug::DrawBB(*fireHydrant->getMesh(), V4(0, 1, 1, 1), fireHydrantWorldSpaceTransform);
			Debug::DrawAABB(*fireHydrant->getMesh(), V4(1, 0, 0, 1), fireHydrantWorldSpaceTransform);
			
			//Implement a gravitational acceleration on the fireHydrant
			fireHydrant->actor->velocity = fireHydrant->actor->velocity + fireHydrant->actor->mass * g;

			//fireHydrant world space
			fireHydrantWorldSpaceTransform = Rotation(V4(0, 0, 1), -0.012f) * Rotation(V4(0, 1, 0), 0.004f) * fireHydrantWorldSpaceTransform
			
			// effect of gravity
			/* *
			Translate(V4(0, -1, 0) * fireHydrant->actor->velocity)*/;

			//fireHydrant view space
			fireHydrantProjectionViewTransform = cam.pv() * fireHydrantWorldSpaceTransform/* * Scalar(V4(.1, .1, .1))*/;

			// cube world space
			// cubeWorldSpaceTransform = cubeWorldSpaceTransform *
			// 							Translate(V4(0, 0, cos(frameIndex / 20.f)));

			// // cube view space
			// cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;

			Ray r(cam.getPos(), (cam.pv() * V4(mouseDirX, mouseDirY, 30.f)).toV3());
			if (isPressed)
			{
				glfwGetCursorPos(this->window->GetHandle(), &mouseDirX, &mouseDirY);
				mouseDirX = mouseDirX; // left -1 right 1.5
				mouseDirY = mouseDirY; // top -1 bottom 3

				// shot a ray

				V3 res;
				if (r.Intersect(res, *plane))
				{
					// std::cout << r.dir.x << r.dir.y << r.dir.z << std::endl;
					// std::cout << "hit at" << res.x << "," << res.y << "," << res.z << std::endl;
				}
				else
				{
					std::cout << "none intersecting" << std::endl;
				}
			}

			for (size_t i = 0; i < 100; i++)
			{
				quadProjectionViewTransform[i] = cam.pv() * quadWorldSpaceTransform[i];
			}

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			fireHydrantScript->setM4(cam.pv(), "m4ProjViewPos");
			// cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			light.bindLight(fireHydrantScript, cam.getPos());
			fireHydrant->DrawScene(fireHydrantProjectionViewTransform, fireHydrantColor);

			light.bindLight(cubeScript, cam.getPos());
			// cube->DrawScene(cubeProjectionViewTransform, cubeColor);

			// for (int i = 0; i < 100; i++)
			// {
			// 	if (plane->pointIsOnPlane(quadWorldSpaceTransform[i].toV3(), .0000001))
			// 	{
			// 		cube->DrawScene(quadProjectionViewTransform[i], fireHydrantColor);
			// 	}
			// }

			// usleep(10000);
			this->window->Update();
			frameIndex++;

			Debug::Render(cam.pv());
			// Debug::Render(cam.pv() * Translate(V4(cam.getPos(), 1)));
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

		// ImGui::SliderFloat("x", &x, -5.f, 5.f);
		// ImGui::SliderFloat("y", &y, -5.f, 5.f);
		// ImGui::SliderFloat("z", &z, -5.f, 5.f);

		// plane->normal = V3(x, y, z);
		ImGui::Text("planeNormal: %.3f\t%.3f\t%.3f", plane->normal.x, plane->normal.y, plane->normal.z);

		ImGui::Text("frames: %d", 1e6 / duration);
		ImGui::Text("Resolution: %.3f %.3f", mouseDirX, mouseDirY);
		ImGui::End();
	}

} // namespace Example
