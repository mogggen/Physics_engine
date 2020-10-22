#include "config.h"
#include "render/ShaderObject.h"

//filetype: .glsl
void ShaderObject::LoadShader(GLchar* vs, GLchar* ps, std::string vsPath, std::string psPath)
{
	std::streampos size;
	//vs
	std::ifstream pathVS(vsPath, std::ios::in | std::ios::binary | std::ios::ate);
	if (pathVS.is_open())
	{
		size = pathVS.tellg();
		vs = new GLchar[size + std::streampos(1)];
		pathVS.seekg(0, std::ios::beg);
		pathVS.read(vs, size);
		vs[size] = '\0';
		pathVS.close();
	}

	//ps
	std::ifstream pathPS(psPath, std::ios::in | std::ios::binary | std::ios::ate);
	if (pathPS.is_open())
	{
		size = pathPS.tellg();
		ps = new GLchar[size + std::streampos(1)];
		pathPS.seekg(0, std::ios::beg);
		pathPS.read(ps, size);
		ps[size] = '\0';
		pathPS.close();
	}

	this->vs = vs;
	this->ps = ps;
}

void ShaderObject::getShaderObject(GLuint vertexShader, GLuint pixelShader, GLuint program)
{
	LoadShader(vs, ps, "textures/vs.glsl", "textures/ps.glsl");

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

ShaderObject::~ShaderObject()
{
	glDeleteProgram(program);
}