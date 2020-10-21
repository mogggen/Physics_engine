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
	std::shared_ptr<MeshResource> Cube(const V4 size, const V4 color);
	MeshResource(Vertex vertices[], int Verticeslength, unsigned int indices[], int indicesLength);
	~MeshResource();
	void Destroy();
	void render();
};