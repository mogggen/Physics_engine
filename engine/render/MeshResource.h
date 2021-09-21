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
	std::shared_ptr<MeshResource> Cube();
	std::shared_ptr<MeshResource> LoadObj(const char* pathToFile);
	MeshResource(Vertex vertices[], int verticeslength, unsigned int indices[], int indicesLength);
	~MeshResource();
	void Destroy();
	void render();
};