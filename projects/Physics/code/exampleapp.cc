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
									  { isPressed = button == GLFW_MOUSE_BUTTON_1 && action;
									  	isLeftPressed = button == GLFW_MOUSE_BUTTON_2 && action;});

		window->SetMouseMoveFunction([this](float64 x, float64 y)
									 {
			if (isLeftPressed)
			{
				senseX = prevX + 0.002 * (x - width / 2);
				senseY = prevY + 0.002 * (y - height / 2);
				cam.setRot(Rotation(V3(0, 1, 0), M_PI) * Rotation(V3(1, 0, 0), senseY) * Rotation(V3(0, 1, 0), senseX));
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
		cam = Camera(90, (float)width / height, 0.01f, 1000.0f);
		cam.setPos(V4(0, 0, 0));
		cam.setRot(V3(0, 1, 0), M_PI);
		// cam.setRot(V4(1, 0, 0), M_PI / 4);

		V4 start[499];
		V4 dirSize[499];
		size_t countLines = 0;

		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);

		float camSpeed = .08f;

		// set identies
		fireHydrantWorldSpaceTransform = Translate(V4());
		fireHydrantProjectionViewTransform = Translate(V4());

		fireHydrantMesh->findbounds();
		

		cubeWorldSpaceTransform = Translate(V4(0, -1, 0, 1));
		cubeProjectionViewTransform = Translate(V4());

		M4 quadWorldSpaceTransform[100];
		M4 quadProjectionViewTransform[100];
		for (size_t i = 0; i < 10; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{
				quadWorldSpaceTransform[i * 10 + j] = Translate(V4(-0.9 + i * 0.2, -1, -0.9 + j * 0.2, 1));
				quadProjectionViewTransform[i * 10 + j] = Translate(V4());
			}			
		}
		
		while (this->window->IsOpen())
		{
			auto startTimer = std::chrono::high_resolution_clock::now();
			//--------------------ImGui section--------------------


			//--------------------math section--------------------
			cam.setPos(cam.getPos() + Normalize(V3((d - a), (q - e), (w - s))) * -camSpeed);
			V3 rayOrigin = cam.getPos() * 1.f;
			
			
			//printf("rayOrigin %.3f %.3f %.3f mousePickingWorldSpace %.3f %.3f %.3f\n", rayOrigin.x, rayOrigin.y, rayOrigin.z, mousePickingWorldSpace.x, mousePickingWorldSpace.y, mousePickingWorldSpace.z);
			
			for (size_t i = 0; i < countLines; i++)
			{
				Debug::DrawLine(start[i], dirSize[i], 1.f, V4(1, 1, 1, 1));
			}
			
			//Debug::DrawBB(*fireHydrant->getMesh(), V4(0, 1, 1, 1), fireHydrantWorldSpaceTransform);
			Debug::DrawAABB(*fireHydrant->getMesh(), V4(1, 0, 0, 1), fireHydrantWorldSpaceTransform);

			Plane XZ2Plane(V3(0, fireHydrantMesh->bottom, 0), V3(0, fireHydrantMesh->bottom, 0));


			V3 res;
			if (isPressed) 
			{
				glfwGetCursorPos(this->window->GetHandle(), &mouseDirX, &mouseDirY);
				// shot a ray
				V4 normalizedDeviceCoordinates(mouseDirX / width * 2 - 1, 1 - mouseDirY / height * 2, 1, 1);
				V4 mousePickingWorldSpace = Inverse(cam.pv()) * normalizedDeviceCoordinates;
				Ray r(rayOrigin, mousePickingWorldSpace.toV3() - rayOrigin);
				
				if (countLines < 0.499)
				{
					start[countLines] = V4(rayOrigin, 1);
					dirSize[countLines++] = mousePickingWorldSpace.toV3() - rayOrigin;
				}


				if (r.Intersect(res, XZ2Plane, mousePickingWorldSpace.toV3()))
				{
					Debug::DrawLine(V4(res - V3(0, 3, 0), 1), V4(res - V3(0, 0, 0), 1), V4(0, 0, 1, 1));
				}
			}

			//Implement a gravitational acceleration on the fireHydrant
			//fireHydrant->actor->velocity = fireHydrant->actor->velocity + fireHydrant->actor->mass * g;

			//fireHydrant world space
			//fireHydrantWorldSpaceTransform = Rotation(V4(0, 0, 1), -0.012f) * Rotation(V4(0, 1, 0), 0.004f) * fireHydrantWorldSpaceTransform
			
			// effect of gravity
			/* *
			Translate(V4(0, -1, 0) * fireHydrant->actor->velocity)*/;

			//fireHydrant view space
			//fireHydrantProjectionViewTransform = cam.pv() * fireHydrantWorldSpaceTransform/* * Scalar(V4(.1, .1, .1))*/;

			// cube world space
			cubeWorldSpaceTransform = Translate(V4(res.x, res.y, res.z, 1));

			// cube view space
			cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform * Scalar(V4(.1, .1, .1));

			

			for (size_t i = 0; i < 100; i++)
			{
				quadProjectionViewTransform[i] = cam.pv() * quadWorldSpaceTransform[i] * Scalar(V4(0.1, 0.001, 0.1));
			}

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//fireHydrantScript->setM4(cam.pv(), "m4ProjViewPos");
			cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			light.bindLight(fireHydrantScript, cam.getPos());
			light.bindLight(cubeScript, cam.getPos());

			//fireHydrant->DrawScene(fireHydrantProjectionViewTransform, fireHydrantColor);
			cube->DrawScene(cubeProjectionViewTransform, cubeColor);

			for (int i = 0; i < 100; i++)
			{
				V3 distToIntersect = V3(res.x - quadWorldSpaceTransform[i].toV3().x, res.y - quadWorldSpaceTransform[i].toV3().y, res.z - quadWorldSpaceTransform[i].toV3().z);
				if (abs(distToIntersect.x) < .1f && abs(distToIntersect.y) < .1f && abs(distToIntersect.z) < .1f)
				{
					printf("hit (%d, %d)\n", i % 10, i / 10);
				}
				cube->DrawScene(quadProjectionViewTransform[i], fireHydrantColor * (abs(distToIntersect.x) < .1f && abs(distToIntersect.y) < 1.f && abs(distToIntersect.z) < .1f ? 1 : 0));
			}

			this->window->Update();
			frameIndex++;

			Debug::Render(cam.pv());
			this->window->SwapBuffers();
			auto finishTimer = std::chrono::system_clock::now();
#ifdef __linux__
			duration = std::chrono::duration_cast<std::chrono::microseconds>(finishTimer - startTimer).count();
#endif
		}
	}

	void ExampleApp::RenderUI()
	{
		bool show = true;
		ImGui::Begin("Mega Cringe", &show, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text("%dx%d %.0f fps", width, height, 1e6f / duration);
		ImGui::End();
	}

} // namespace Example
