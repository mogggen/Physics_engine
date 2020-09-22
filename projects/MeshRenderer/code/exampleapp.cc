//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

const GLchar* vs =
"#version 430\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec4 color;\n"
"uniform mat4 m4;\n"
"layout(location=0) out vec4 Color;\n"
"void main()\n"
"{\n"
"	gl_Position = m4 * vec4(pos, 1);\n"
"	Color = color;\n"
"}\n";

const GLchar* ps =
"#version 430\n"
"layout(location=0) in vec4 color;\n"
"out vec4 Color;\n"
"void main()\n"
"{\n"
"	Color = color;\n"
"}\n";

using namespace Display;
namespace Example
{
	MeshResource::MeshResource(Vertice vertices[], int Verticeslength, unsigned int indices[], int indicesLength) : indices(indicesLength)
	{
		glGenBuffers(1, &this->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * 7 * Verticeslength, vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &this->indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesLength, indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void MeshResource::render()
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float32) * 7, NULL);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float32) * 7, (GLvoid*)(sizeof(float32) * 3));
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/// <summary>
	/// Destroy glBuffers
	/// </summary>
	void MeshResource::Destroy()
	{
		glDeleteBuffers(1, &this->indexBuffer);
		glDeleteBuffers(1, &this->vertexBuffer);
	}

	MeshResource::~MeshResource()
	{
		Destroy();
	}

	Camera::Camera(V4 pos) : pos(pos)
	{

	}

	M4 Camera::LookAt(V4 target, V4 up)
	{
		V4 zaxis = Normalize(pos - target);
		V4 xaxis = Normalize(Cross(Normalize(up), zaxis));
		V4 yaxis = Cross(zaxis, xaxis);

		M4 translation;
		translation[0][0] = 1;
		translation[1][1] = 1;
		translation[2][2] = 1;
		translation[3][3] = 1;

		translation[3][0] = -pos.x;
		translation[3][1] = -pos.y;
		translation[3][2] = -pos.z;
		
		M4 rotation;
		rotation[0][0] = xaxis.x;
		rotation[1][0] = xaxis.y;
		rotation[2][0] = xaxis.z;

		rotation[0][1] = yaxis.x;
		rotation[1][1] = yaxis.y;
		rotation[2][1] = yaxis.z;

		rotation[0][2] = zaxis.x;
		rotation[1][2] = zaxis.y;
		rotation[2][2] = zaxis.z;

		return rotation * translation;
	}

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
		window->SetKeyPressFunction([this](int32, int32, int32, int32)
		{
			this->window->Close();
		});



		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			// setup vertex shader
			this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
			GLint length = static_cast<GLint>(std::strlen(vs));
			glShaderSource(this->vertexShader, 1, &vs, &length);
			glCompileShader(this->vertexShader);

			// get error log
			GLint shaderLogSize;
			glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
			if (shaderLogSize > 0)
			{
				GLchar* buf = new GLchar[shaderLogSize];
				glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
				printf("[SHADER COMPILE ERROR]: %s", buf);
				delete[] buf;
			}

			// setup pixel shader
			this->pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
			length = static_cast<GLint>(std::strlen(ps));
			glShaderSource(this->pixelShader, 1, &ps, &length);
			glCompileShader(this->pixelShader);

			// get error log
			shaderLogSize;
			glGetShaderiv(this->pixelShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
			if (shaderLogSize > 0)
			{
				GLchar* buf = new GLchar[shaderLogSize];
				glGetShaderInfoLog(this->pixelShader, shaderLogSize, NULL, buf);
				printf("[SHADER COMPILE ERROR]: %s", buf);
				delete[] buf;
			}

			// create a program object
			this->program = glCreateProgram();
			glAttachShader(this->program, this->vertexShader);
			glAttachShader(this->program, this->pixelShader);
			glLinkProgram(this->program);
			glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
			if (shaderLogSize > 0)
			{
				GLchar* buf = new GLchar[shaderLogSize];
				glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
				printf("[PROGRAM LINK ERROR]: %s", buf);
				delete[] buf;
			}

			// setup vbo
			cube = cube->Cube(V4(1, 1, 1, 1), V4(1, 0, 1, 1));
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	*/

	MeshResource* MeshResource::Cube(V4 size, V4 color)
	{
		Vertice vertices[] =
		{
			Vertice
			{
				V3(-0.5f * size[0], -0.5f * size[1], -0.5f * size[2]),
				color
			},
			Vertice
			{
				V3(-0.5 * size[0], 0.5f * size[1], -0.5f * size[2]),
				color
			},
			Vertice
			{
				V3(0.5f * size[0], -0.5f * size[1], -0.5f * size[2]),
				color
			},
			Vertice
			{
				V3(0.5f * size[0], 0.5f * size[1], -0.5f * size[2]),
				color
			},

			Vertice
			{
				V3(-0.5f * size[0], -0.5f * size[1], 0.5f * size[2]),
				color
			},
			Vertice
			{
				V3(-0.5 * size[0], 0.5f * size[1], 0.5f * size[2]),
				color
			},
			Vertice
			{
				V3(0.5f * size[0], -0.5f * size[1], 0.5f * size[2]),
				color
			},
			Vertice
			{
				V3(0.5f * size[0], 0.5f * size[1], 0.5f * size[2]),
				color
			},
		};

		unsigned int indices[] =
		{
		0, 1, 2,	//front
		1, 2, 3,

		4, 5, 6,	//back
		5, 6, 7,

		0, 1, 4,	//left
		1, 4, 5,

		6, 7, 2,	//right
		7, 2, 3,

		1, 3, 5,	//top
		3, 5, 7,

		0, 2, 4,	//bottom
		2, 4, 6,
		};

		return new MeshResource(vertices, sizeof(vertices) / sizeof(Vertice), indices, sizeof(vertices) / sizeof(unsigned int));
	}

	void
		ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		float angle = 0;
		int width, height;
		window->GetSize(width, height);
		Camera cam(V4(0, 0, 0));

		M4 scene;
		M4 m;
		M4 v = cam.LookAt(V4(0, 0, 0), V4(0, 1, 0));
		M4 p = projection(60, width / height, 0.10f, 100.0f);;
		while (this->window->IsOpen())
		{
			angle += 0.006f;
			m = Translate(V4(sinf(angle), 0, -angle)) *
				Rotation(V4(0, 0, 1), M_PI / 6) *
				Rotation(V4(1, 0, 0), -M_PI / 6) *
				Rotation(V4(0, 1, 0), angle) *
				Scalar(0.5f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			// do stuff
			glUseProgram(this->program);

			auto loc = glGetUniformLocation(program, "m4");
			scene = p * (/*v **/ m);
			glUniformMatrix4fv(loc, 1, GL_TRUE, (float*)&scene);
			cube->render();

			this->window->SwapBuffers();
		}
	}

} // namespace Example