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

		unsigned char* img = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb);
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

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextureResource::BindTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	
	void ShaderObject::Init(GLuint vertexShader, GLuint pixelShader, GLuint program)
	{
		// setup vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLint length = static_cast<GLint>(std::strlen(vs));
		glShaderSource(vertexShader, 1, &vs, &length);
		glCompileShader(vertexShader);

		// get error log
		GLint shaderLogSize;
 		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(vertexShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}

		// setup pixel shader
		pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		length = static_cast<GLint>(std::strlen(ps));
		glShaderSource(pixelShader, 1, &ps, &length);
		glCompileShader(pixelShader);

		// get error log
		glGetShaderiv(pixelShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(pixelShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}

		// create a program object
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetProgramInfoLog(program, shaderLogSize, NULL, buf);
			printf("[PROGRAM LINK ERROR]: %s", buf);
			delete[] buf;
		}
		this->program = program;
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

			//vs and ps defined here
			shaderObject = new ShaderObject;
			shaderObject->Init(this->vertexShader, this->pixelShader, this->program);

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
		V4 top(0, 255, 0); //red
		V4 back(128, 66, 128); //gray
		V4 left(0, 0, 255); //blue
		V4 right(255, 0, 0); //red
		V4 front(255, 165, 0); //orange
		V4 bottom(64, 224, 208); //turquoise
		
		float factor = .01f;
		top = top * factor;
		back = back * factor;
		left = left * factor;
		right = right * factor;
		front = front * factor;
		bottom = bottom * factor;

		Vertex vertices[] =
		{
			//Mesh:	(0,1)	(1,1)	Tex:(0,0)	(1,0)
			//		(0,0)	(1,0)		(0,1)	(1,1)

			//xyz
			//000
			Vertex //back : 0
			{
				V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
				back,
				1, 1
			},
			Vertex //left : 1
			{
				V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
				left,
				0, 1
			},
			Vertex //bottom : 2
			{
				V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
				bottom,
				1, 0
			},

			//100
			Vertex //back : 3
			{
				V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
				back,
				0, 1
			},
			Vertex //right : 4
			{
				V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
				right,
				1, 1
			},
			Vertex //bottom : 5
			{
				V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
				bottom,
				0, 0
			},

			//010
			Vertex //back : 6
			{
				V3(-.5 * size[0], .5f * size[1], -.5f * size[2]),
				back,
				1, 0
			},
			Vertex //left : 7
			{
				V3(-.5 * size[0], .5f * size[1], -.5f * size[2]),
				left,
				0, 0
			},
			Vertex //top : 8
			{
				V3(-.5 * size[0], .5f * size[1], -.5f * size[2]),
				top,
				0, 0
			},

			//110
			Vertex //back : 9
			{
				V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
				back,
				0, 0
			},
			Vertex //right : 10
			{
				V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
				right,
				1, 0
			},
			Vertex //top : 11
			{
				V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
				top,
				1, 0
			},

			//001
			Vertex //left : 12
			{
				V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
				left,
				1, 1
			},
			Vertex //front : 13
			{
				V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
				front,
				0, 1
			},
			Vertex //bottom : 14
			{
				V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
				bottom,
				1, 1
			},

			//101
			Vertex //right : 15
			{
				V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
				right,
				0, 1
			},
			Vertex //front : 16
			{
				V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
				front,
				1, 1
			},
			Vertex //bottom : 17
			{
				V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
				bottom,
				0, 1
			},

			//011
			Vertex // left : 18
			{
				V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
				left,
				1, 0
			},
			Vertex // front : 19
			{
				V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
				front,
				0, 0
			},
			Vertex // top : 20
			{
				V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
				top,
				0, 1
			},

			//111
			Vertex // right : 21
			{
				V3(.5f * size[0], .5f * size[1], .5f * size[2]),
				right,
				0, 0
			},
			Vertex // front : 22
			{
				V3(.5f * size[0], .5f * size[1], .5f * size[2]),
				front,
				1, 0
			},
			Vertex // top : 23
			{
				V3(.5f * size[0], .5f * size[1], .5f * size[2]),
				top,
				1, 1
			},
		};

		unsigned int indices[]
		{
			0, 3, 6,
			3, 6, 9, //back

			1, 12, 7,
			7, 12, 18, //left

			4, 15, 10,
			10, 15, 21, //right

			13, 16, 19,
			16, 19, 22, //front

			20, 8, 23,
			8, 23, 11, //top

			2, 5, 14,
			5, 14, 17, //bottom
		};

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
		texture.LoadFromFile("textures/perfect.jpg");
		Camera cam(90, (float)width / height, 0.10f, 100.0f);
		bool d = true;
		char i = 0;
		M4 scene;
		M4 m;
		M4 v = cam.projectionViewMatrix();
		while (this->window->IsOpen())
		{
			step += 0.006f;
			m = Translate(V4(0, 0, -5)) * Rotation(V4(1, 1, 0), step);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();

			// do stuff
			glUseProgram(shaderObject->program);
			texture.BindTexture();

			scene = v * m;
			auto loc = glGetUniformLocation(shaderObject->program, "m4");
			glUniformMatrix4fv(loc, 1, GL_TRUE, (float*)&scene);
			cube->render();

			this->window->SwapBuffers();
		}
	}

} // namespace Example