#pragma once
#include <GL/glew.h>
#include "stb_image.h"

class TextureResource
{
	GLuint texture;
public:
	void BindTexture();
	void LoadFromFile(const char* filename);
};