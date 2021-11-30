﻿//------------------------------------------------------------------------------
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
#include <ctime>
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
			for (char i = 0; i < 4; i++)
				std::cout << v.data[i] << (i == 3 ? ")\n" : ", ");
		}
	}

	bool
		ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;

		//assign ExampleApp variables
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
			}
		});

		window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
		{
			isPressed = button == GLFW_MOUSE_BUTTON_1 && action;
		});

		window->SetMouseMoveFunction([this](float64 x, float64 y)
		{
			if (isPressed)
			{
				senseX = prevX + (0.002 * (x - width / 2));
				senseY = prevY + (0.002 * (y - height / 2));
				cubeProjectionViewTransform = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
				fireHydrantProjectionViewTransform = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
			}
		});
		

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			//MeshResource
			fireHydrantMesh = MeshResource::LoadObj("textures/fireHydrant.obj");

			//TextureResource
			fireHydrantTexture = std::make_shared<TextureResource>("textures/cubepic.png");

			//shaderResource
			fireHydrantScript = std::make_shared<ShaderResource>();
			fireHydrantScript->getShaderResource(this->vertexShader, this->pixelShader, this->program);
			
			//Actor
			Actor temp;
			Actor* fireHydrantActor = &temp;

			//GraphicNode
			fireHydrant = std::make_shared<GraphicNode>(fireHydrantMesh, fireHydrantTexture, fireHydrantScript, fireHydrantActor);



			//MeshResource
			cubeMesh = MeshResource::LoadObj("textures/cube.obj");

			cubeTexture = std::make_shared<TextureResource>("textures/red.png");
			
			//shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->getShaderResource(this->vertexShader, this->pixelShader, this->program, "textures/vs.glsl", "textures/psNoTexture.glsl");
			// note: bindTexture() still requires a texture, but just won't use it
			
			//Actor
			Actor temp2;
			Actor* cubeActor = &temp2;
			
			//GraphicNode
			cube = std::make_shared<GraphicNode>(cubeMesh, cubeTexture, cubeScript, cubeActor);



			//MeshResource
			quadMesh = MeshResource::LoadObj("textures/quad.obj");
			
			//Actor
			Actor temp3;
			Actor* quadActor = &temp3;
			
			//GraphicNode
			quad = std::make_shared<GraphicNode>(quadMesh, cubeTexture, cubeScript, quadActor);

			this->window->SetUiRender([this]()
			{
				this->RenderUI();
			});
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

		Camera cam(90, (float)width / height, 0.01f, 100.0f);
		cam.setPos(V4(0, 4, 3));
		cam.setRot(V4(0, 1, 0), M_PI);
		
		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);
		
		float camSpeed = .08f;

		// set identies
		fireHydrantWorldSpaceTransform = fireHydrantProjectionViewTransform = Translate(V4());
		
		cubeWorldSpaceTransform = cubeProjectionViewTransform = Translate(V4());

		M4 quadWorldSpaceTransform[100];
		M4 quadProjectionViewTransform[100];
		for (size_t i = 0; i < 10; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{
				quadWorldSpaceTransform[i * 10 + j] = quadProjectionViewTransform[i] = Translate(V4());
				quadWorldSpaceTransform[i * 10 + j] = Translate(V4(i * 2, j * 2, 0));	
			}
		}

		

		while (this->window->IsOpen())
		{
			//--------------------ImGui section--------------------
			
			

			//--------------------math section--------------------
			cam.setPos(cam.getPos() + Normalize(V4((d - a), (q - e), (w - s))) * -camSpeed);
			plane = new Plane(V3(), V3(0, 0, -1));
			// std::cout << "frame " << frameIndex << std::endl;

			// fireHydrant->getTexture()->LoadFromFile();

			// Implement a gravitational acceleration on the fireHydrant
			// fireHydrant->actor->velocity = fireHydrant->actor->velocity + fireHydrant->actor->mass * g;
			
			// fireHydrant world space
			// fireHydrantWorldSpaceTransform = fireHydrantWorldSpaceTransform *
			// Translate(V4(0, -1, 0) * fireHydrant->actor->velocity);

			// fireHydrant view space
			// fireHydrantProjectionViewTransform = cam.pv() * fireHydrantWorldSpaceTransform * Scalar(V4(.1, .1, .1));

			
			// cube world space
			cubeWorldSpaceTransform = cubeWorldSpaceTransform *
			Translate(V4(0, 0, cos(frameIndex / 20.f)));

			// // cube view space
			cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;

			// equation
			double mouseWorldX, mouseWorldY;
			
			if (isPressed)
			{
				glfwGetCursorPos(this->window->GetHandle(), &mouseWorldX, &mouseWorldY);
				mouseWorldX = (mouseWorldX / this->width);
				mouseWorldY = (mouseWorldY / this->width);
				//std::cout << "x:" << mouseWorldX << " y:" << mouseWorldY << std::endl;

				//shot a ray
				Ray r(cam.getPos(), V3(mouseWorldX, 0, mouseWorldY));
				
				V3 res;
				if (r.Intersect(res, *plane))
				{
					std::cout << r.dir.x << r.dir.y << r.dir.z << std::endl;
					// std::cout << "hit at" << res.x << "," << res.y << "," << res.z << std::endl;
				}
			}

			for (size_t i = 0; i < 100; i++)
			{
				quadProjectionViewTransform[i] = cam.pv() * quadWorldSpaceTransform[i];
			}

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			

			// fireHydrantScript->setM4(cam.pv(), "m4ProjViewPos");
			cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			light.bindLight(fireHydrantScript, cam.getPos());
			fireHydrant->DrawScene(fireHydrantProjectionViewTransform, fireHydrantColor);

			light.bindLight(cubeScript, cam.getPos());
			cube->DrawScene(cubeProjectionViewTransform, cubeColor);
			
			
			
			for (int i = 0; i < 100; i++)
			{
				if (plane->pointIsOnPlane(quadWorldSpaceTransform[i].toV3(), plane->MARGIN))
				{
					cube->DrawScene(quadProjectionViewTransform[i], fireHydrantColor);
				}
			}
			
			// usleep(10000);
			this->window->Update();
			frameIndex++;

			this->window->SwapBuffers();
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
		ImGui::SliderFloat("margin ", &plane->MARGIN, 1.e-7f, 1.e-4f);
		ImGui::Text("frames: %d", frameIndex);

		ImGui::End();
	}

} // namespace Example