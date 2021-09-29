﻿//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "stb_image.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <cstring>

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
			//cube = MeshResource::Cube();

			cube = MeshResource::LoadObj("textures/sphere.obj");

			//Load Susanne from file
			//monkey = monkey->LoadObj("textures/monke susanne.obj");

			//TextureResource
			std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>("textures/cubepic.png");

			//shaderObject
			shaderResource = std::make_shared<ShaderResource>();
			shaderResource->getShaderResource(this->vertexShader, this->pixelShader, this->program);

			//shaderObjectSusanne = std::make_shared<ShaderResource>();
			//shaderObjectSusanne->getShaderResource(this->vertexShader, this->pixelShader, this->program);
			
			//GraphicNode
			node = std::make_shared<GraphicNode>(cube, texture, shaderResource, Translate(V4Zero));

			//susanne = std::make_shared<GraphicNode>(monkey, texture, shaderObjectSusanne, Translate(V4Zero));

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	*/

	void Print(const M4& m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			V4& v = m[i];
			std::cout << '(';
			for (char i = 0; i < 4; i++)
				std::cout << round(v.data[i]) << (i == 3 ? ")\n" : ", ");
		}
	}

	void
		ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		node->getTexture()->LoadFromFile();
		Camera cam(90, (float)width / height, 0.01f, 100.0f);
		cam.setPos(V4(0, 0, -3));
		cam.setRot(V4(0, 1, 0), M_PI);
		
		float speed = .08f;

		M4 scene;
		V4 color(1, 1, 1, 1);
		
		while (this->window->IsOpen())
		{
			Em = Em * Translate(Normalize(V4(float(d - a), float(e - q), float(w - s))) * speed);
			scene = cam.pv() * (Em * Evp) * Translate(V4Zero) * Scalar(V4(-1, -1, 1)); // scaling because i can
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			shaderResource->setM4(cam.pv(), "m4ProjViewPos");
			node->DrawScene(scene, color);
			// susanne->DrawScene(scene, color);
			this->window->SwapBuffers();
		}
	}

} // namespace Example