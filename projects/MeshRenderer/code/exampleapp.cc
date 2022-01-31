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
		one = two = three = four = five = six = seven = eight = false;
		window->GetSize(width, height);
		Em = Translate(V4());
		window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
		{
			//deltatime
			switch (keycode)
			{
			case GLFW_KEY_ESCAPE: window->Close(); break;
			case GLFW_KEY_1: one = action; break;
			case GLFW_KEY_2: two = action; break;
			case GLFW_KEY_3: three = action; break;
			case GLFW_KEY_4: four = action; break;
			case GLFW_KEY_5: five = action; break;
			case GLFW_KEY_6: six = action; break;
			case GLFW_KEY_7: seven = action; break;
			case GLFW_KEY_8: eight = action; break;

			case GLFW_KEY_ENTER: enter = action; break;
			}
		});

		window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
		{

		});

		window->SetMouseMoveFunction([this](float64 x, float64 y)
		{
		});
		

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			//MeshResource

			cube = MeshResource::LoadObj("textures/roulette.obj");

			//TextureResource
			std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>("textures/cubepic.png");

			//shaderObject
			shaderResource = std::make_shared<ShaderResource>();
			shaderResource->getShaderResource(this->vertexShader, this->pixelShader, this->program);
			
			//GraphicNode
			node = std::make_shared<GraphicNode>(cube, texture, shaderResource, Translate(V4Zero));

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	*/

	void RotUp(M4& model, char value, float angularVelocity=0.08f)
	{
		Rotation(V4(1, 0, 0, 0), angularVelocity);
	}

	void RotDown(M4& model, char value, float angularVelocity=0.08f)
	{
		Rotation(V4(1, 0, 0, 0), -angularVelocity);
	}

	void Print(M4 m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			V4 v = m[i];
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
		cam.setPos(V4(0, 0, 3));
		cam.setRot(V4(0, 1, 0), M_PI);
		Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);
		
		float speed = .08f;

		V4 color(1, 1, 1, 1);

		srand((unsigned int)time(NULL));
		M4 canvasModels[8];
		char canvasValues[8];
		
		for (size_t i = 0; i < 8; i++)
		{
			canvasValues[i] = (char)rand() % sizeof(char);
			canvasModels[i] = cam.pv() * Translate(V4(0, 0, 0, 1));
		}
		

		while (this->window->IsOpen())
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			shaderResource->setM4(cam.pv(), "projViewMat");
			light.bindLight(shaderResource, cam.getPos());
			node->DrawScene(Inverse(cam.pv()), color);
			this->window->SwapBuffers();
		}
	}

} // namespace Example