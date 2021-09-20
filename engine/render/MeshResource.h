#pragma once
#include "Vertex.h"
#include <GL/glew.h>
#include <iostream>
#include <memory>

class MeshResource
{
	GLint indices;
	GLuint vertexBuffer;
	GLuint indexBuffer;
public:
	std::shared_ptr<MeshResource> Cube();
	std::shared_ptr<MeshResource> LoadObj();
	MeshResource(Vertex vertices[], int verticeslength, unsigned int indices[], int indicesLength);
	~MeshResource();
	void Destroy();
	void render();
};