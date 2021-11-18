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
		Em = Evp = Translate(V4());
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
				Evp = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
			}
		});
		

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			//MeshResource

			cube = MeshResource::LoadObj("textures/fireHydrant.obj");

			//TextureResource
			std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>("textures/cubepic.png");

			//shaderObject
			shaderResource = std::make_shared<ShaderResource>();
			shaderResource->getShaderResource(this->vertexShader, this->pixelShader, this->program);
			
			Actor temporary;
			temporary.velocity = 0.f;
			temporary.mass = 1.f;
			temporary.transform = Translate(V4(0, 0, 0));
			
			Actor* dummy = &temporary;

			//GraphicNode
			node = std::make_shared<GraphicNode>(cube, texture, shaderResource, dummy);

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

		const float g = -9.806f;
		node->getTexture()->LoadFromFile();

		Camera cam(90, (float)width / height, 0.01f, 100.0f);
		cam.setPos(V4(0, 0, -3));
		cam.setRot(V4(0, 1, 0), M_PI);
		
		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);
		
		float speed = .08f;

		// a scene matrix to represent the world position relative to the camera
		M4 scene;
		V4 color(1, 1, 1, 1);
		
		while (this->window->IsOpen())
		{
			// set frame cap
			glfwSetTime(1000.0 / 60);

			// Implement gravity, without collison detection
			node->actor->velocity = node->actor->velocity + node->actor->mass * g;

			node->actor->transform = node->actor->transform * (Translate(V4(1, 1, 1) * node->actor->velocity));
			printf("\n");
			Print(node->actor->transform); 
			Em = Em * Translate(Normalize(V4(float(d - a), float(e - q), float(w - s))) * -speed);
			scene = cam.pv() * (Em * Evp) * Translate(V4()) * Scalar(V4(.1, .1, .1));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			shaderResource->setM4(cam.pv(), "m4ProjViewPos");
			light.bindLight(shaderResource, cam.getPos());
			node->DrawScene(scene, color);
			this->window->SwapBuffers();
		}
	}

} // namespace Example