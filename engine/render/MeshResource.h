#pragma once
#include "Vertex.h"
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <memory>

class MeshResource
{
	GLuint indices;
	GLuint vertexBuffer;
	GLuint indexBuffer;

public:

	std::vector<V3> positions;
	std::vector<unsigned> indicesAmount;
	std::vector<V2> texels;
	std::vector<V3> normals;
	std::vector<Vertex> vertices;

	V3 center_of_mass;
	
	// end points
	V3 min, max;
	//float left;
	//float bottom;
	//float front;

	//float right;
	//float top;
	//float back;

	static std::shared_ptr<MeshResource> Cube();
	bool findCenterOfMass();
	bool findbounds();
	static std::shared_ptr<MeshResource> LoadObj(const char *pathToFile,
		std::vector<uint32>& _indices,
		std::vector<V3>& _positions,
		std::vector<V2>& _texels,
		std::vector<V3>& _normals,
		std::vector<Vertex>& _vertices);
	MeshResource();
	MeshResource(Vertex vertices[], uint32_t verticesLength, uint32_t indices[], uint32_t indicesLength);
	~MeshResource();
	void Destroy();
	void render();
};