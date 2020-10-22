#pragma once
#include <GL/glew.h>
#include <fstream>

struct ShaderObject
{
	GLchar* vs;
	GLchar* ps;
	GLuint program;
	void getShaderObject(GLuint vs, GLuint ps, GLuint prog);
	void LoadShader(GLchar* vs, GLchar* ps, std::string vsPath, std::string psPath);
};