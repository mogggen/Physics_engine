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
"uniform vec4 scal;\n"
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

		Vertice quad[] =
		{
			Vertice
			{
				V3(-0.5f, -0.5f, 0),
				V4(1, 0, 0, 1),
			},
			Vertice
			{
				V3(-0.5, 0.5f,	0),
				V4(0, 1, 0, 1),
			},
			Vertice
			{
				V3(0.5f, -0.5f, 0),
				V4(0, 0, 1, 1),
			},
			Vertice
			{
				V3(0.5f, 0.5f, 0),
				V4(0, 0, 0, 1),
			},

			Vertice
			{
				V3(-0.5f, -0.5f, 1),
				V4(1, 0, 0, 1),
			},
			Vertice
			{
				V3(-0.5, 0.5f,	1),
				V4(0, 1, 0, 1),
			},
			Vertice
			{
				V3(0.5f, -0.5f, 1),
				V4(0, 0, 1, 1),
			},
			Vertice
			{
				V3(0.5f, 0.5f, 1),
				V4(0, 0, 0, 1),
			},
		};
		
		unsigned int indices[36] =
		{0, 1, 2,	//front
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

		// setup vbo
		quadrilateral = new MeshResource(quad, 8, indices, 36);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/

M4 Translate(V3 pos)
{
	M4 temp;
	temp[0] = V4(1, 0, 0, pos[0]);
	temp[1] = V4(0, 1, 0, pos[1]);
	temp[2] = V4(0, 0, 1, pos[2]);
	temp[3] = V4(0, 0, 0, 1);
	return temp;
}

M4 Translate(float x, float y, float z)
{
	M4 temp;
	temp[0] = V4(1, 0, 0, x);
	temp[1] = V4(0, 1, 0, y);
	temp[2] = V4(0, 0, 1, z);
	temp[3] = V4(0, 0, 0, 1);
	return temp;
}

M4 Scalar(float s)
{
	M4 temp;
	temp[0] = V4(s, 0, 0, 0);
	temp[1] = V4(0, s, 0, 0);
	temp[2] = V4(0, 0, s, 0);
	temp[3] = V4(0, 0, 0, 1);
	return temp;
}

M4 projectiveViewMatrix(float fov, float aspect, float n, float f)
{
	float s = 1.0f / (tan(fov / 2) * M_PI / 180.0f);
	M4 temp;
	temp[0] = V4(s, 0, 0, 0);
	temp[1] = V4(0, s, 0, 0);
	temp[2] = V4(0, 0, -f / (f - n), -1);
	temp[3] = V4(0, 0, -(f * n) / (f - n), 0);
	return temp;
}

void
ExampleApp::Run()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//V4 line(1, 1, 1);
	float angle = 0;
	char i = 0, j = 0;
	M4 matrix1;
	M4 matrix2;
	while (this->window->IsOpen())
	{
		angle += 0.006f;
		matrix1 = //projectiveViewMatrix(M_PI / 3, 4.0f / 3, 0.1f, 100) *
			Translate(cos(angle), 0, 0) *
			Rotation(V4(0, 0, 1), M_PI / 6) *
			Rotation(V4(1, 0, 0), -M_PI / 6) *
			Rotation(V4(0, 1, 0), angle) *
			Scalar(0.5f);
			
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();

		// do stuff
		glUseProgram(this->program);
		
		auto loc = glGetUniformLocation(program, "m4");
		glUniformMatrix4fv(loc, 1, GL_TRUE, (float*)&matrix1);
		quadrilateral->render();
		/*glUniformMatrix4fv(loc, 1, GL_TRUE, (float*)&matrix2);
		quadrilateral->render();*/

		this->window->SwapBuffers();
	}
}

} // namespace Example