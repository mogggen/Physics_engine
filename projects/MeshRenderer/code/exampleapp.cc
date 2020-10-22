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
		isRotate = false;
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
		});

		window->SetMouseMoveFunction([this](float64 x, float64 y)
		{
			if (isRotate)
			{
				senseX = 0.002f * (x - width / 2);
				senseY = 0.002f * (y - height / 2);
				Evp = Rotation(V4(1, 0, 0), senseY) * Rotation(V4(0, 1, 0), senseX);
			}
		});
		

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			//MeshResource
			cube = cube->Cube(V4(1, 1, 1), V4(1, 1, 1));

			//TextureResource
			std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>();

			//shaderObject
			shaderObject = std::make_shared<ShaderObject>();
			shaderObject->getShaderObject(this->vertexShader, this->pixelShader, this->program);

			
			//GraphicNode
			node = std::make_shared<GraphicNode>(cube, texture, shaderObject, Translate(V4(0, 0, 0)));

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
		node->getTexture()->LoadFromFile("textures/perfect.jpg");
		Camera cam(90, (float)width / height, 0.01f, 100.0f);
		cam.setPos(V4(0, 0, -3));
		cam.setRot(V4(0, 1, 0), M_PI);
		float speed = .08f;
		M4 scene; V4 color(1, 1, 1);
		while (this->window->IsOpen())
		{
			Em = Em * Translate(Normalize(V4(d - a, e - q, w - s)) * speed);
			scene = cam.pv() * (Em * Evp) * Translate(V4(0, 0, 0)) * Scalar(V4(-1, -1, 1));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();
			node->DrawScene(scene, color);
			this->window->SwapBuffers();
		}
	}

} // namespace Example