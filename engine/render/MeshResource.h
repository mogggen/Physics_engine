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
	std::vector<V3> positions;
	V4 centerOfMass;
	
	// end points
	float left;
	float bottom;
	float front;

	float right;
	float top;
	float back;

	static std::shared_ptr<MeshResource> Cube();
	bool findCenterOfMass();
	bool findbounds();
	static std::vector<V3> LoadVerticesFromFile(const char *pathToFile);
	static std::shared_ptr<MeshResource> LoadObj(const char *pathToFile);
	MeshResource();
	MeshResource(Vertex vertices[], uint32_t verticesLength, uint32_t indices[], uint32_t indicesLength);
	~MeshResource();
	void Destroy();
	void render();
};