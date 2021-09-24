#pragma once
#include <GL/glew.h>
#include <fstream>
#include "core/MathLibrary.h"

struct ShaderObject
{
	GLchar* vs;
	GLchar* ps;
	GLuint program;

	public:
	~ShaderObject();
	void getShaderObject(GLuint vs, GLuint ps, GLuint program);
	void LoadShader(GLchar* vs, GLchar* ps, std::string vsPath, std::string psPath);
	void bindShaderObject();
	
	void setVec3(V3 vecIn, std::string paramNam);
	void setVec4(V4 vecIn, std::string paramNam);
	void setMat4(M4 matIn, std::string paramNam);
	void setFloat(float floatIn, std::string paramNam);

};