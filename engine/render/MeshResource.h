#pragma once
#include "Vertex.h"
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <memory>

class MeshResource
{
	GLint indices;
	GLuint vertexBuffer;
	GLuint indexBuffer;
public:
	static std::shared_ptr<MeshResource> Cube();
	static std::shared_ptr<MeshResource> LoadObj(const char* pathToFile);
	static std::shared_ptr<MeshResource> LoadGLTF(const std::string& filePath);
	MeshResource(Vertex vertices[], uint32_t verticeslength, uint32_t indices[], uint32_t indicesLength);
	~MeshResource();
	void Destroy();
	void render();
};