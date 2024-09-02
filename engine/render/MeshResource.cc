#include "config.h"
#include "render/MeshResource.h"
#include <string>
#include <stdio.h>
#include <inttypes.h>
#include <iostream>
#include <fstream>

MeshResource::MeshResource()
{

}

MeshResource::MeshResource(Vertex vertices[], uint32_t verticesLength, uint32_t indices[], uint32_t indicesLength) : indices(indicesLength)
{
	glGenBuffers(1, &this->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticesLength, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &this->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesLength, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void MeshResource::render()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 7));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 9));

	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



std::shared_ptr<MeshResource> MeshResource::Cube(
	std::vector<uint32>& _indices,
	std::vector<V3>& _pos,
	std::vector<V3>& _normals,
	std::vector<Vertex>& _vertices,
	std::vector<Face>& _faces)
{
	V4 top(0, 255, 0, 100);		  // red
	V4 back(128, 66, 128, 100);	  // gray
	V4 left(0, 0, 255, 100);	  // blue
	V4 right(255, 0, 0, 100);	  // red
	V4 front(255, 165, 0, 100);	  // orange
	V4 bottom(64, 224, 208, 100); // turquoise

	float factor = .01f;
	top = top * factor;
	back = back * factor;
	left = left * factor;
	right = right * factor;
	front = front * factor;
	bottom = bottom * factor;

	Vertex vertices[] = // world points
	{
		// Mesh:	(0,1)	(1,1)	Tex:(0,0)	(1,0)
		//		(0,0)	(1,0)		(0,1)	(1,1)

		// xyz
		// 000
		Vertex // back : 0
		{
			V3(-.5f, -.5f, -.5f), // position, will transform later with the projection matrix
			back,				  // raw color 0-1 : black-white
			V2(1, 1),			  // texture position, when the indicies are added, combined with this data, the orientation and size of the texture will make sense
			V3(0, 0, -1)
		},
		Vertex // left : 1
		{
			V3(-.5f, -.5f, -.5f),
			left,
			V2(0, 1),
			V3(-1, 0, 0)},
		Vertex // bottom : 2
		{
			V3(-.5f, -.5f, -.5f),
			bottom,
			V2(1, 0),
			V3(0, -1, 0)},

		// 100
		Vertex // back : 3
		{
			V3(.5f, -.5f, -.5f),
			back,
			V2(0, 1),
			V3(0, 0, -1)},
		Vertex // right : 4
		{
			V3(.5f, -.5f, -.5f),
			right,
			V2(1, 1),
			V3(1, 0, 0)},
		Vertex // bottom : 5
		{
			V3(.5f, -.5f, -.5f),
			bottom,
			V2(0, 0),
			V3(0, -1, 0)},

		// 010
		Vertex // back : 6
		{
			V3(-.5f, .5f, -.5f),
			back,
			V2(1, 0),
			V3(0, 0, -1)},
		Vertex // left : 7
		{
			V3(-.5f, .5f, -.5f),
			left,
			V2(0, 0),
			V3(-1, 0, 0)},
		Vertex // top : 8
		{
			V3(-.5f, .5f, -.5f),
			top,
			V2(0, 0),
			V3(0, 1, 0)},

		// 110
		Vertex // back : 9
		{
			V3(.5f, .5f, -.5f),
			back,
			V2(0, 0),
			V3(0, 0, -1)},
		Vertex // right : 10
		{
			V3(.5f, .5f, -.5f),
			right,
			V2(1, 0),
			V3(1, 0, 0)},
		Vertex // top : 11
		{
			V3(.5f, .5f, -.5f),
			top,
			V2(1, 0),
			V3(0, 1, 0)},

		// 001
		Vertex // left : 12
		{
			V3(-.5f, -.5f, .5f),
			left,
			V2(1, 1),
			V3(-1, 0, 0)},
		Vertex // front : 13
		{
			V3(-.5f, -.5f, .5f),
			front,
			V2(0, 1),
			V3(0, 0, 1)},
		Vertex // bottom : 14
		{
			V3(-.5f, -.5f, .5f),
			bottom,
			V2(1, 1),
			V3(0, -1, 0)},

		// 101
		Vertex // right : 15
		{
			V3(.5f, -.5f, .5f),
			right,
			V2(0, 1),
			V3(1, 0, 0)},
		Vertex // front : 16
		{
			V3(.5f, -.5f, .5f),
			front,
			V2(1, 1),
			V3(0, 0, 1)},
		Vertex // bottom : 17
		{
			V3(.5f, -.5f, .5f),
			bottom,
			V2(0, 1),
			V3(0, -1, 0)},

		// 011
		Vertex // left : 18
		{
			V3(-.5f, .5f, .5f),
			left,
			V2(1, 0),
			V3(-1, 0, 0)},
		Vertex // front : 19
		{
			V3(-.5f, .5f, .5f),
			front,
			V2(0, 0),
			V3(0, 0, 1)},
		Vertex // top : 20
		{
			V3(-.5f, .5f, .5f),
			top,
			V2(0, 1),
			V3(0, 1, 0)},

		// 111
		Vertex // right : 21
		{
			V3(.5f, .5f, .5f),
			right,
			V2(0, 0),
			V3(1, 0, 0)},
		Vertex // front : 22
		{
			V3(.5f, .5f, .5f),
			front,
			V2(1, 0),
			V3(0, 0, 1)},
		Vertex // top : 23
		{
			V3(.5f, .5f, .5f),
			top,
			V2(1, 1),
			V3(0, 1, 0)},
	};

	uint32_t indices[] // World point's relations to form triangles and surfaces with razterisation
	{
		0, 3, 6,
		3, 6, 9, // back

		1, 12, 7,
		7, 12, 18, // left

		4, 15, 10,
		10, 15, 21, // right

		13, 16, 19,
		16, 19, 22, // front

		20, 8, 23,
		8, 23, 11, // top

		2, 5, 14,
		5, 14, 17, // bottom
	};
	
    {
        Face f = Face();
        f.vertices = { vertices[0].pos, vertices[3].pos, vertices[6].pos, vertices[9].pos };
        f.normal = vertices[0].normal;
        _faces.push_back(f);
    }

    {
        Face f = Face();
        f.vertices = { vertices[1].pos, vertices[12].pos, vertices[7].pos, vertices[18].pos };
        f.normal = vertices[1].normal;
        _faces.push_back(f);
    }

    {
        Face f = Face();
        f.vertices = { vertices[4].pos, vertices[15].pos, vertices[10].pos, vertices[21].pos };
        f.normal = vertices[4].normal;
        _faces.push_back(f);
    }

    {
        Face f = Face();
        f.vertices = { vertices[13].pos, vertices[16].pos, vertices[19].pos, vertices[22].pos };
        f.normal = vertices[13].normal;
        _faces.push_back(f);
    }

    {
        Face f = Face();
        f.vertices = { vertices[20].pos, vertices[8].pos, vertices[23].pos, vertices[11].pos };
        f.normal = vertices[20].normal;
        _faces.push_back(f);
    }

    {
        Face f = Face();
        f.vertices = { vertices[2].pos, vertices[5].pos, vertices[14].pos, vertices[17].pos };
        f.normal = vertices[2].normal;
        _faces.push_back(f);
    }

	for (size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++)
	{
		_pos.push_back(vertices[i].pos);
		_normals.push_back(vertices[i].normal);
		_vertices.push_back(vertices[i]);
		_indices.push_back(indices[i]);
	}

	return std::make_shared<MeshResource>(vertices, sizeof(vertices) / sizeof(Vertex), indices, sizeof(indices) / sizeof(uint64_t));
}

std::pair<V3, V3> MeshResource::find_bounds()
{
	// can't be less than the minimum value of 'undefined', so let's not, mate?
	min[0] = max[0] = positions[0][0];
	min[1] = max[1] = positions[0][1];
	min[2] = max[2] = positions[0][2];

	// I should actually do this for all 6 of them but.. nah

	for (size_t i = 1; i < positions.size(); i++)
	{
		if (positions[i][0] < min[0])
			min[0] = positions[i][0];

		if (positions[i][1] < min[1])
			min[1] = positions[i][1];

		if (positions[i][2] < min[2])
			min[2] = positions[i][2];
		
		if (positions[i][0] > max[0])
			max[0] = positions[i][0];

		if (positions[i][1] > max[1])
			max[1] = positions[i][1];

		if (positions[i][2] > max[2])
			max[2] = positions[i][2];
	}

	return { min, max };
}


inline static std::vector<std::string> Split(std::string in, const std::string& sep=" ")
{
	std::vector<std::string> result;
	size_t pos = 0;
	std::string token;
	while ((pos = in.find(sep)) != std::string::npos) {
		token = in.substr(0, pos);
		result.push_back(token);
		in.erase(0, pos + sep.length());
	}
	result.push_back(in);
	return result;
}

std::shared_ptr<MeshResource> MeshResource::LoadObj(const char* pathToFile,
	std::vector<uint32>& _indices,
	std::vector<V3>& _pos,
	std::vector<V2>& _texels,
	std::vector<V3>& _normals,
	std::vector<Vertex>& _vertices,
	std::vector<Face>& _faces)
{
	using namespace std;
	ifstream fs(pathToFile);
	if (!fs)
	{
		printf("file not found with path \"./%s\"\n", pathToFile);
		return nullptr;
	}

	string parsedLine;
	while (getline(fs, parsedLine))
	{
		vector<string> args = Split(parsedLine);
		string token = args[0];
		args.erase(args.begin());

		if (token == "v") {
			V3 pos;
			for (size_t i = 0; i < args.size(); ++i)
				pos[i] = stof(args[i]);
			_pos.push_back(pos);
		} else
		if (token == "vt") {
			V2 texel;
			for (size_t i = 0; i < args.size(); ++i)
				texel[i] = stof(args[i]);
			_texels.push_back(texel);
		} else
		if (token == "vn") {
			V3 norm;
			for (size_t i = 0; i < args.size(); ++i)
				norm[i] = stof(args[i]);
			_normals.push_back(norm);
		} else
		if (token == "f") {
			Face tempFace;
			tempFace.vertices = {};
			for (size_t i = 0; i < args.size(); i++) {
				vector<string> pos_tex_nor = Split(args[i], "/");
				Vertex v = Vertex();
				v.pos = pos_tex_nor[0] != "" ? _pos[stoi(pos_tex_nor[0]) - 1] : V3();
				v.rgba = V4(1, 1, 1, 1);
				v.texel = pos_tex_nor[1] != "" ? _texels[stoi(pos_tex_nor[1]) - 1] : V2();
				v.normal = pos_tex_nor[2] != "" ? _normals[stoi(pos_tex_nor[2]) - 1] : V3();
				_vertices.push_back(v);
				_indices.push_back(stoi(pos_tex_nor[0]) - 1);
				
				tempFace.vertices.push_back(v.pos);
				tempFace.normal = _normals[stoi(pos_tex_nor[2]) - 1];
			}
			_faces.push_back(tempFace);
		}
	}
	
	std::cout << "Parsed wavefront" << pathToFile << "\n";
	return std::make_shared<MeshResource>(&_vertices[0], _vertices.size(), &_indices[0], _indices.size());
}

/// <summary>
/// Destroy glBuffers
/// </summary>
void MeshResource::Destroy()
{
	glDeleteBuffers(1, &this->indexBuffer);
	glDeleteBuffers(1, &this->vertexBuffer);
}

MeshResource::~MeshResource()
{
	Destroy();
}
