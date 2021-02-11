#pragma once
#include <GL/glew.h>
#include <fstream>
#include <vector>
#include <memory>
#include "core/MathLibrary.h"
#include "render/Camera.h"

struct ShaderObject
{
	GLchar* vs;
	GLchar* ps;
	GLuint program;
	
	ShaderObject();
	void getShaderObject(GLuint vs, GLuint ps, GLuint prog);
	void LoadShader(GLchar* vs, GLchar* ps, std::string vsPath, std::string psPath);
	void BindShader();

	//setters
	void setV3(V3 val, std::string uniform);
	void setV4(V4 val, std::string uniform);
	void setM4(M4 val, std::string uniform);
	void setF(float val, std::string uniform);

	~ShaderObject();
};