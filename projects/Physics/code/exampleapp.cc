//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
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
			isRotate = button == GLFW_MOUSE_BUTTON_1 && action;
			if (!isRotate)
			{
				prevX = senseX;
				prevY = senseY;
			}
		});

		window->SetMouseMoveFunction([this](float64 x, float64 y)
		{
			if (isRotate)
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
			cube = std::make_shared<GraphicNode>(cubeMesh, fireHydrantTexture, cubeScript, cubeActor);



			//MeshResource
			quadMesh = MeshResource::LoadObj("textures/quad.obj");
			
			//Actor
			Actor temp3;
			Actor* quadActor = &temp3;
			
			//GraphicNode
			quad = std::make_shared<GraphicNode>(quadMesh, cubeTexture, cubeScript, quadActor);

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

		const float g = -9.806e-3f;
		fireHydrantTexture->LoadFromFile();

		Camera cam(90, (float)width / height, 0.01f, 100.0f);
		cam.setPos(V4(0, 0, -3));
		cam.setRot(V4(0, 1, 0), M_PI);
		
		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);
		
		float speed = .08f;
		uint frameIndex = 0;

		// set identies
		fireHydrantWorldSpaceTransform = fireHydrantProjectionViewTransform = Translate(V4());
		
		cubeWorldSpaceTransform = cubeProjectionViewTransform = Translate(V4());

		M4 quadWorldSpaceTransform[10];
		M4 quadProjectionViewTransform[10];
		for (size_t i = 0; i < 10; i++)
		{
			quadWorldSpaceTransform[i] = quadProjectionViewTransform[i] = Translate(V4());
			quadWorldSpaceTransform[i] = Translate(V4(i * 10, 0, 0));
		}

		while (this->window->IsOpen())
		{
			// set frame cap
			glfwSetTime(1000.0 / 60);

			// Implement a gravitational acceleration on the fireHydrant
			fireHydrant->actor->velocity = fireHydrant->actor->velocity + fireHydrant->actor->mass * g;
			
			// fireHydrant world space
			fireHydrantWorldSpaceTransform = fireHydrantWorldSpaceTransform *
			Translate(V4(0, -1, 0) * fireHydrant->actor->velocity) *
			Translate(Normalize(V4(float(d - a), float(q - e), float(w - s))) * -speed);

			// fireHydrant view space
			fireHydrantProjectionViewTransform = cam.pv() * fireHydrantWorldSpaceTransform * Scalar(V4(.1, .1, .1));

			
			// cube world space
			cubeWorldSpaceTransform = cubeWorldSpaceTransform *
			Translate(V4(0, -1, 0) * cube->actor->velocity) *
			Translate(Normalize(V4(float(d - a), float(q - e), float(w - s))) * -speed);

			// cube view space
			cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;


			for (int i = 0; i < 10; i++)
			{
				// quad world space
				quadWorldSpaceTransform[i] = quadWorldSpaceTransform[i] *
				Translate(V4(0, 0, 0)) *
				Translate(Normalize(V4(float(d - a), float(q - e), float(w - s))) * -speed);

				quadProjectionViewTransform[i] = cam.pv() * quadWorldSpaceTransform[i];
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			fireHydrantScript->setM4(cam.pv(), "m4ProjViewPos");
			cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			light.bindLight(fireHydrantScript, cam.getPos());
			fireHydrant->DrawScene(fireHydrantProjectionViewTransform, fireHydrantColor);

			light.bindLight(cubeScript, cam.getPos());
			//cube->DrawScene(cubeProjectionViewTransform, cubeColor);

			for (int i = 0; i < 10; i++)
			{
				if (i % 2 == 0)
					cube->DrawScene(quadProjectionViewTransform[i], cubeColor);
				else
					cube->DrawScene(quadProjectionViewTransform[i], fireHydrantColor);
			}
			

			this->window->SwapBuffers();
			usleep(10000);
			frameIndex++;
		}
	}

} // namespace Example