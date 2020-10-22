#include "config.h"
#include "render/MeshResource.h"

MeshResource::MeshResource(Vertex vertices[], int Verticeslength, unsigned int indices[], int indicesLength) : indices(indicesLength)
{
	glGenBuffers(1, &this->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Verticeslength, vertices, GL_STATIC_DRAW);

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float32) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float32) * 7));

	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::shared_ptr<MeshResource> MeshResource::Cube(V4 size, V4 color)
{
	V4 top(0, 255, 0); //red
	V4 back(128, 66, 128); //gray
	V4 left(0, 0, 255); //blue
	V4 right(255, 0, 0); //red
	V4 front(255, 165, 0); //orange
	V4 bottom(64, 224, 208); //turquoise

	float factor = .01f;
	top = top * factor;
	back = back * factor;
	left = left * factor;
	right = right * factor;
	front = front * factor;
	bottom = bottom * factor;

	Vertex vertices[] =
	{
		//Mesh:	(0,1)	(1,1)	Tex:(0,0)	(1,0)
		//		(0,0)	(1,0)		(0,1)	(1,1)

		//xyz
		//000
		Vertex //back : 0
		{
			V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
			back,
			1, 1
		},
		Vertex //left : 1
		{
			V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
			left,
			0, 1
		},
		Vertex //bottom : 2
		{
			V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
			bottom,
			1, 0
		},

		//100
		Vertex //back : 3
		{
			V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
			back,
			0, 1
		},
		Vertex //right : 4
		{
			V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
			right,
			1, 1
		},
		Vertex //bottom : 5
		{
			V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
			bottom,
			0, 0
		},

		//010
		Vertex //back : 6
		{
			V3(-.5f * size[0], .5f * size[1], -.5f * size[2]),
			back,
			1, 0
		},
		Vertex //left : 7
		{
			V3(-.5f * size[0], .5f * size[1], -.5f * size[2]),
			left,
			0, 0
		},
		Vertex //top : 8
		{
			V3(-.5f * size[0], .5f * size[1], -.5f * size[2]),
			top,
			0, 0
		},

		//110
		Vertex //back : 9
		{
			V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
			back,
			0, 0
		},
		Vertex //right : 10
		{
			V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
			right,
			1, 0
		},
		Vertex //top : 11
		{
			V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
			top,
			1, 0
		},

		//001
		Vertex //left : 12
		{
			V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
			left,
			1, 1
		},
		Vertex //front : 13
		{
			V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
			front,
			0, 1
		},
		Vertex //bottom : 14
		{
			V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
			bottom,
			1, 1
		},

		//101
		Vertex //right : 15
		{
			V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
			right,
			0, 1
		},
		Vertex //front : 16
		{
			V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
			front,
			1, 1
		},
		Vertex //bottom : 17
		{
			V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
			bottom,
			0, 1
		},

		//011
		Vertex // left : 18
		{
			V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
			left,
			1, 0
		},
		Vertex // front : 19
		{
			V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
			front,
			0, 0
		},
		Vertex // top : 20
		{
			V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
			top,
			0, 1
		},

		//111
		Vertex // right : 21
		{
			V3(.5f * size[0], .5f * size[1], .5f * size[2]),
			right,
			0, 0
		},
		Vertex // front : 22
		{
			V3(.5f * size[0], .5f * size[1], .5f * size[2]),
			front,
			1, 0
		},
		Vertex // top : 23
		{
			V3(.5f * size[0], .5f * size[1], .5f * size[2]),
			top,
			1, 1
		},
	};

	unsigned int indices[]
	{
		0, 3, 6,
		3, 6, 9, //back

		1, 12, 7,
		7, 12, 18, //left

		4, 15, 10,
		10, 15, 21, //right

		13, 16, 19,
		16, 19, 22, //front

		20, 8, 23,
		8, 23, 11, //top

		2, 5, 14,
		5, 14, 17, //bottom
	};

	MeshResource* temp1 = new MeshResource(MeshResource(vertices, sizeof(vertices) / sizeof(Vertex), indices, sizeof(indices) / sizeof(unsigned int)));
	std::shared_ptr<MeshResource> temp(temp1);
	return temp;
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