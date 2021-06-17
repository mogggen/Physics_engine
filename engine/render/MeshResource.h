#pragma once
#include "Vertex.h"
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <memory>

class MeshResource
{
	GLint indicesLength;
	GLuint vertexBuffer;
	GLuint indexBuffer;
public:
	MeshResource(Vertex vertices[], unsigned int indices[], int Verticeslength, int indicesLength);
	MeshResource(MeshResource& meshResource);

	//Meshes
	std::shared_ptr<MeshResource> Cube(const V3 pos, const V4 size, const V4 color);
	std::shared_ptr<MeshResource> CustomObj(std::string fileName);

	void Destroy();
	void render();

	~MeshResource();
};