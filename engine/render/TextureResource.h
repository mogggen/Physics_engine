#pragma once
#include <GL/glew.h>
#include "stb_image.h"
#include <string>

class TextureResource
{
	GLuint texture;
	std::string file;
	//unsigned char* buf;
	//int widthImg, heightImg;
public:
	TextureResource(std::string file);
	//TextureResource(unsigned char* buf, const int& widthImg, const int& heightImg);
	~TextureResource();

	void setFile(std::string file);
	void setBuf(const unsigned char* buf);
	
	void BindTexture();
	void LoadFromBuffer(void* buf, const int& widthImg, const int& heightImg);
	void LoadFromFile();
};