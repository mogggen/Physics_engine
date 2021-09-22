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

std::shared_ptr<MeshResource> MeshResource::Cube()
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

	Vertex vertices[] = // world points
	{
		//Mesh:	(0,1)	(1,1)	Tex:(0,0)	(1,0)
		//		(0,0)	(1,0)		(0,1)	(1,1)

		//xyz
		//000
		Vertex //back : 0
		{
			V3(-.5f, -.5f, -.5f), // position, will transform later with the projection matrix
			back, // raw color 0-1 : black-white
			V2(1, 1) // texture position, when the indicies are added, combined with this data, the orientation and size of the texture will make sense
		},
		Vertex //left : 1
		{
			V3(-.5f, -.5f, -.5f),
			left,
			V2(0, 1)
		},
		Vertex //bottom : 2
		{
			V3(-.5f, -.5f, -.5f),
			bottom,
			V2(1, 0)
		},

		//100
		Vertex //back : 3
		{
			V3(.5f, -.5f, -.5f),
			back,
			V2(0, 1)
		},
		Vertex //right : 4
		{
			V3(.5f, -.5f, -.5f),
			right,
			V2(1, 1)
		},
		Vertex //bottom : 5
		{
			V3(.5f, -.5f, -.5f),
			bottom,
			V2(0, 0)
		},

		//010
		Vertex //back : 6
		{
			V3(-.5f, .5f, -.5f),
			back,
			V2(1, 0)
		},
		Vertex //left : 7
		{
			V3(-.5f, .5f, -.5f),
			left,
			V2(0, 0)
		},
		Vertex //top : 8
		{
			V3(-.5f, .5f, -.5f),
			top,
			V2(0, 0)
		},

		//110
		Vertex //back : 9
		{
			V3(.5f, .5f, -.5f),
			back,
			V2(0, 0)
		},
		Vertex //right : 10
		{
			V3(.5f, .5f, -.5f),
			right,
			V2(1, 0)
		},
		Vertex //top : 11
		{
			V3(.5f, .5f, -.5f),
			top,
			V2(1, 0)
		},

		//001
		Vertex //left : 12
		{
			V3(-.5f, -.5f, .5f),
			left,
			V2(1, 1)
		},
		Vertex //front : 13
		{
			V3(-.5f, -.5f, .5f),
			front,
			V2(0, 1)
		},
		Vertex //bottom : 14
		{
			V3(-.5f, -.5f, .5f),
			bottom,
			V2(1, 1)
		},

		//101
		Vertex //right : 15
		{
			V3(.5f, -.5f, .5f),
			right,
			V2(0, 1)
		},
		Vertex //front : 16
		{
			V3(.5f, -.5f, .5f),
			front,
			V2(1, 1)
		},
		Vertex //bottom : 17
		{
			V3(.5f, -.5f, .5f),
			bottom,
			V2(0, 1)
		},

		//011
		Vertex // left : 18
		{
			V3(-.5f, .5f, .5f),
			left,
			V2(1, 0)
		},
		Vertex // front : 19
		{
			V3(-.5f, .5f, .5f),
			front,
			V2(0, 0)
		},
		Vertex // top : 20
		{
			V3(-.5f, .5f, .5f),
			top,
			V2(0, 1)
		},

		//111
		Vertex // right : 21
		{
			V3(.5f, .5f, .5f),
			right,
			V2(0, 0)
		},
		Vertex // front : 22
		{
			V3(.5f, .5f, .5f),
			front,
			V2(1, 0)
		},
		Vertex // top : 23
		{
			V3(.5f, .5f, .5f),
			top,
			V2(1, 1)
		},
	};

	unsigned int indices[] // World point's relations to form triangles and surfaces with razterisation
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

std::shared_ptr<MeshResource> MeshResource::LoadObj(const char* pathToFile)
{
	std::vector<char> buf;
	FILE* fs = fopen(pathToFile, "r"); // "textures/cube.obj"	
	
	unsigned long long numOfIndices = 0;
	std::vector<uint32_t> indices;
	std::vector<V3> coords;
	std::vector<V2> texels;
	std::vector<V3> normals;
	std::vector<Vertex> vertices; // complete package

	if (fs)
	{
		uint32_t vertexIndex = 1u;
		uint32_t textureIndex = 1u;
		uint32_t normalIndex = 1u;


		while (true)
		{
			if (!fscanf(fs, "%s", buf)) break;
			
			if (buf[0] == 'v' && buf[1] == ' ')
			{
				V3 nextCoordinate;
				if (fscanf(fs, "%f %f %f", &nextCoordinate.x, &nextCoordinate.y, &nextCoordinate.z) == 3)
				{
					coords.push_back(nextCoordinate);
				}
				else
				{
					std::cerr << "missing arguments in vertex, expected 3" << std::endl;
				}
			}

			else if (buf[0] == 'v' && buf[1] == 't' && buf[2] == ' ')
			{
				V2 nextTexel;
				if (fscanf(fs, "%f %f %f", &nextTexel.x, &nextTexel.y) == 2)
				{
					texels.push_back(nextTexel);
				}
				else
				{
					std::cerr << "missing arguments in vertex, expected 3" << std::endl;
				}
				texels.push_back(nextTexel);
			}

			else if (buf[0] == 'v' && buf[1] == 'n' && buf[2] == ' ')
			{
				V3 nextNormal;
			}

			else if (buf[0] == 'f' && buf[1] == ' ')
			{
				uint32_t vertexIndex, textureIndex, normalIndex;
				char a[16], b[16], c[16], d[16];
				uint8_t argc = fscanf(fs, "%s %s %s %s", &a, &b, &c, &d);

				int listOfIndices[4][3];
				for (size_t i = 0; i < 2; i++)
				{
					sscanf(a, "%d/ %d/ %d/", &listOfIndices[i][0], &listOfIndices[i][1], &listOfIndices[i][2]);

					vertices.push_back(Vertex(coords[(listOfIndices[i][0]) - 1], V4(1, 1, 1, 1), texels[(listOfIndices[i][1]) - 1], normals[(listOfIndices[i][2]) - 1]));

					indices.push_back(numOfIndices);

					numOfIndices++;
				}
				
				if (argc == 4)
				{
					if (d[0] != 'f' && d[0] != '#' && d[0] != ' ' && d[0] != '\n')
					{

						sscanf(d, "%d/ %d/ %d/", &listOfIndices[3][0], &listOfIndices[3][1], &listOfIndices[3][2]);

						vertices.push_back(Vertex(coords[(listOfIndices[3][0]) - 1], V4(1, 1, 1, 1), texels[(listOfIndices[3][1]) - 1], normals[(listOfIndices[3][2]) - 1]));

						indices.push_back(numOfIndices - 3);
						indices.push_back(numOfIndices - 1);
						indices.push_back(numOfIndices);
						
						numOfIndices++;
					}
				}
			}
		}
	}
	else
	{
		printf("file not found with path \"./%s\"", pathToFile);
	}
	fclose(fs);

	MeshResource* temp1 = new MeshResource(MeshResource(vertices, vertices.size(), indices, indices.size()));
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