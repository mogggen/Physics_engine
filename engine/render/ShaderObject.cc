#include "config.h"
#include "render/ShaderObject.h"

ShaderObject::ShaderObject()
{

}

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
	LoadShader(vs, ps, "core/render/vs.glsl", "core/render/ps.glsl");
	//LoadShader(vs, ps, "textures/shaders/vs.glsl", "textures/shaders/ps.glsl");

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



void ShaderObject::setF(float floatIn, std::string uniform) {
	glUseProgram(program);
	glUniform1f(glGetUniformLocation(program, uniform.c_str()), floatIn);
}

void ShaderObject::setV3(V3 vec3, std::string uniform) {
	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, uniform.c_str()), 1, (float*)&vec3);
}

void ShaderObject::setV4(V4 vec4, std::string uniform) {
	glUseProgram(program);
	glUniform4fv(glGetUniformLocation(program, uniform.c_str()), 1, (float*)&vec4);
}

void ShaderObject::setM4(M4 m4, std::string uniform) {
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, uniform.c_str()), 1, GL_FALSE, (float*)&(m4));
}

void ShaderObject::BindShader() {
	glUseProgram(program);
}

ShaderObject::~ShaderObject()
{
	glDeleteProgram(program);
}