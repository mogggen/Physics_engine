//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "stb_image.h"
#include "exampleapp.h"
#include <cstring>

const GLchar* vs =
"#version 430\n"

"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec4 color;\n"
"layout(location=2) in vec2 texturesIn;\n"

"out vec4 Colors;\n"
"out vec2 texturesOut;\n"

"uniform mat4 m4;\n"
"uniform vec4 colorVector;\n"
"uniform sampler2D textureArray;\n"

"void main()\n"
"{\n"
"	gl_Position = m4 * vec4(pos, 1);\n"
"	Colors = color;\n"
"	texturesOut = texturesIn;\n"
"}\n";

const GLchar* ps =
"#version 430\n"

"layout(location=0) in vec4 Colors;\n"
"layout(location=1) in vec2 texturesOut;\n"

"uniform sampler2D textureArray;\n"

"out vec4 Color;\n"

"void main()\n"
"{\n"
"	Color = texture(textureArray, texturesOut) * Colors;\n"
"}\n";

using namespace Display;
namespace Example
{
	MeshResource::MeshResource(Vertex vertices[], int Verticeslength, unsigned int indices[], int indicesLength) : indices(indicesLength)
	{
		glGenBuffers(1, &this->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Verticeslength, vertices, GL_STATIC_DRAW);

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
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float32) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float32) * 7));
		
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

	Camera::Camera(float fov, float aspect, float n, float f) :  fov(fov),  aspect(aspect),  n(n),  f(f)
	{
		pos = V4(0, 0, 0);
		dir = V4(0, 1, 0);
	}

	void Camera::setPos(V4 pos)
	{
		this->pos = pos;
	}

	void Camera::setRot(V4 dir, float rad)
	{
		this->dir = dir;
		this->rad = rad;
	}

	M4 Camera::projectionViewMatrix()
	{
		return projection(fov, aspect, n, f) * Translate(pos) * Rotation(dir, rad);
	}

	void TextureResource::LoadFromFile(const char* filename)
	{
		int width, height, nrChannels;

		unsigned char* img = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb_alpha);
		if (img == nullptr)
		{
			printf("Image loaded incorrectly");
			exit(1);
		}

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		}

		else if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextureResource::BindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, texture);
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
			cube = cube->Cube(V4(1, 1, 1), V4(1, 1, 1));
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	*/

	MeshResource* MeshResource::Cube(V4 size, V4 color)
	{
		//x,y
		float bottomLeft[2] = { 0, 0 };
		float bottomRight[2] = { 1, 0 };

		float topLeft[2] = { 0, 1 };
		float topRight[2] = { 1, 1 };

		//1	3
		//0	2

		Vertex vertices[] =
		{
			//0 = -.5f
			//1 = .5f
			//xyz

			//000
			Vertex
			{
				V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
				color,
				bottomRight[0],
				bottomRight[1]
			},
			Vertex
			{
				V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
				color,
				bottomRight[0],
				bottomRight[1]
			},
			Vertex
			{
				V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
				color,
				bottomRight[0],
				bottomRight[1]
			},

			//100
			Vertex
			{
				V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
				color,
				bottomLeft[0],
				bottomLeft[1]
			},
			Vertex
			{
				V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
				color,
				bottomLeft[0],
				bottomLeft[1]
			},
			Vertex
			{
				V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
				color,
				bottomLeft[0],
				bottomLeft[1]
			},

			//010
			Vertex
			{
				V3(-.5 * size[0], .5f * size[1], -.5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(-.5 * size[0], .5f * size[1], -.5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(-.5 * size[0], .5f * size[1], -.5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},

			//110
			Vertex
			{
				V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
				color,
				topLeft[0],
				topLeft[1]
			},
			Vertex
			{
				V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
				color,
				topLeft[0],
				topLeft[1]
			},
			Vertex
			{
				V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
				color,
				topLeft[0],
				topLeft[1]
			},

			//001
			Vertex
			{
				V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},

			//101
			Vertex
			{
				V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},

			//011
			Vertex
			{
				V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
			Vertex
			{
				V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},

			//111
			Vertex
			{
				V3(.5f * size[0], .5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
				Vertex
			{
				V3(.5f * size[0], .5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
				Vertex
			{
				V3(.5f * size[0], .5f * size[1], .5f * size[2]),
				color,
				topRight[0],
				topRight[1]
			},
		};

		unsigned int indices[24];

		for (char i = 0; i < sizeof(indices) / sizeof(unsigned int); i++)
		{
			indices[i] = 0;
			indices[i + 1] = 0;
			indices[i + 2]
		}

		return new MeshResource(vertices, sizeof(vertices) / sizeof(Vertex), indices, sizeof(indices) / sizeof(unsigned int));
	}

	void
		ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		float step = 0;
		int width, height;
		window->GetSize(width, height);
		TextureResource texture;
		texture.LoadFromFile("textures/BETTER.jpg");
		Camera cam(90, width / height, 0.10f, 100.0f);
		bool d = true;
		char i = 0;
		M4 scene;
		M4 m;
		M4 v = cam.projectionViewMatrix();
		while (this->window->IsOpen())
		{
			step += 0.006f;
			m = Translate(V4(0, 0, -5)) * Rotation(V4(0, 1, 0), step);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			// do stuff
			glUseProgram(this->program);
			texture.BindTexture();

			scene = v * m;
			auto loc = glGetUniformLocation(program, "m4");
			glUniformMatrix4fv(loc, 1, GL_TRUE, (float*)&scene);
			cube->render();

			this->window->SwapBuffers();
		}
	}

} // namespace Example