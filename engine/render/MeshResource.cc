#include "config.h"
#include "render/MeshResource.h"

MeshResource::MeshResource(Vertex vertices[], unsigned int indices[], int Verticeslength, int indicesLength) : indicesLength(indicesLength)
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

MeshResource::MeshResource(MeshResource& mesh) : vertexBuffer(mesh.vertexBuffer), indexBuffer(mesh.indexBuffer), indicesLength(mesh.indicesLength)
{

}

void MeshResource::render()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);


	// these Attributes should be in the same order for vs.glsl (which is fragment shader also)
	//and ps.glsl
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	//attribute, length, "parametrar", offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GLfloat) * 7));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GLfloat) * 9));
	glDrawElements(GL_TRIANGLES, indicesLength, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::shared_ptr<MeshResource> MeshResource::Cube(V3 pos, V4 size, V4 color)
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
			V2(1, 1),
			V3(0, 0, 0)
		},
		Vertex //left : 1
		{
			V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
			left,
			V2(0, 1),
			V3(0, 0, 0)
		},
		Vertex //bottom : 2
		{
			V3(-.5f * size[0], -.5f * size[1], -.5f * size[2]),
			bottom,
			V2(1, 0),
			V3(0, 0, 0)
		},

		//100
		Vertex //back : 3
		{
			V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
			back,
			V2(0, 1),
			V3(0, 0, 0)
		},
		Vertex //right : 4
		{
			V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
			right,
			V2(1, 1),
			V3(0, 0, 0)
		},
		Vertex //bottom : 5
		{
			V3(.5f * size[0], -.5f * size[1], -.5f * size[2]),
			bottom,
			V2(0, 0),
			V3(0, 0, 0)
		},

		//010
		Vertex //back : 6
		{
			V3(-.5f * size[0], .5f * size[1], -.5f * size[2]),
			back,
			V2(1, 0),
			V3(0, 0, 0)
		},
		Vertex //left : 7
		{
			V3(-.5f * size[0], .5f * size[1], -.5f * size[2]),
			left,
			V2(0, 0),
			V3(0, 0, 0)
		},
		Vertex //top : 8
		{
			V3(-.5f * size[0], .5f * size[1], -.5f * size[2]),
			top,
			V2(0, 0),
			V3(0, 0, 0)
		},

		//110
		Vertex //back : 9
		{
			V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
			back,
			V2(0, 0),
			V3(0, 0, 0)
		},
		Vertex //right : 10
		{
			V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
			right,
			V2(1, 0),
			V3(0, 0, 0)
		},
		Vertex //top : 11
		{
			V3(.5f * size[0], .5f * size[1], -.5f * size[2]),
			top,
			V2(1, 0),
			V3(0, 0, 0)
		},

		//001
		Vertex //left : 12
		{
			V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
			left,
			V2(1, 1),
			V3(0, 0, 0)
		},
		Vertex //front : 13
		{
			V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
			front,
			V2(0, 1),
			V3(0, 0, 0)
		},
		Vertex //bottom : 14
		{
			V3(-.5f * size[0], -.5f * size[1], .5f * size[2]),
			bottom,
			V2(1, 1),
			V3(0, 0, 0)
		},

		//101
		Vertex //right : 15
		{
			V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
			right,
			V2(0, 1),
			V3(0, 0, 0)
		},
		Vertex //front : 16
		{
			V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
			front,
			V2(1, 1),
			V3(0, 0, 0)
		},
		Vertex //bottom : 17
		{
			V3(.5f * size[0], -.5f * size[1], .5f * size[2]),
			bottom,
			V2(0, 1),
			V3(0, 0, 0)
		},

		//011
		Vertex // left : 18
		{
			V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
			left,
			V2(1, 0),
			V3(0, 0, 0)
		},
		Vertex // front : 19
		{
			V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
			front,
			V2(0, 0),
			V3(0, 0, 0)
		},
		Vertex // top : 20
		{
			V3(-.5f * size[0], .5f * size[1], .5f * size[2]),
			top,
			V2(0, 1),
			V3(0, 0, 0)
		},

		//111
		Vertex // right : 21
		{
			V3(.5f * size[0], .5f * size[1], .5f * size[2]),
			right,
			V2(0, 0),
			V3(0, 0, 0)
		},
		Vertex // front : 22
		{
			V3(.5f * size[0], .5f * size[1], .5f * size[2]),
			front,
			V2(1, 0),
			V3(0, 0, 0)
		},
		Vertex // top : 23
		{
			V3(.5f * size[0], .5f * size[1], .5f * size[2]),
			top,
			V2(1, 1),
			V3(0, 0, 0)
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

	MeshResource* temp1 = new MeshResource(MeshResource(vertices, indices, sizeof(vertices) / sizeof(Vertex), sizeof(indices) / sizeof(unsigned int)));
	std::shared_ptr<MeshResource> temp(temp1);
	return temp;
}

std::shared_ptr<MeshResource> MeshResource::CustomObj(std::string fileName)
{
	FILE* myfile = fopen(("textures/OBJs/" + fileName + ".obj").c_str(), "r");
	std::vector<V3> coords; // Vertex Coords
	std::vector<V2> texels; // Vertex Textures
	std::vector<V3> normals; // Vertex normals
	std::vector<Vertex> vertices;
	int verticesLength = 0;

	// Vertex Indices
	std::vector<unsigned int> indices;

	char buf[256];
	bool indexCheck = true;
	bool scanning = true;

	// If file is found
	if (myfile != NULL)
	{
		while (scanning) {
			// If it should scan or not
			if (indexCheck) {
				// If end of file is reached
				if ((fscanf(myfile, "%256s", buf) <= 0)) {
					break;
				}
			}

			// Vertex Coords
			if (buf[0] == 'v' && buf[1] == '\0') {
				float x, y, z;
				if (fscanf(myfile, "%f %f %f", &x, &y, &z) == 3) {
					coords.push_back(V3(x, y, z));
				}
				else { std::cout << "Vertex coords broken" << std::endl; }
			}

			// Vertex Textures
			else if (buf[0] == 'v' && buf[1] == 't' && buf[2] == '\0') {
				float x, y;
				if (fscanf(myfile, "%f %f", &x, &y) == 2) {
					texels.push_back(V2(x, 1 - y));
				}
				else { std::cout << "Vertex texture coords broken" << std::endl; }
			}

			// Vertex Normals
			else if (buf[0] == 'v' && buf[1] == 'n' && buf[2] == '\0') {
				float x, y, z;
				if (fscanf(myfile, "%f %f %f", &x, &y, &z) == 3) {
					normals.push_back(V3(x, y, z));
				}
				else { std::cout << "Vertex normal coords broken" << std::endl; }
			}

			// Vertices
			else if (buf[0] == 'f' && buf[1] == '\0') {
				indexCheck = true;
				char x[64], y[64], z[64], w[64];
				int reads = 0;

				reads = fscanf(myfile, "%s %s %s %s", &x, &y, &z, &w);

				// Find and add vertices depending on indices
				int indexList[4][3];
				sscanf(x, "%d/ %d/ %d/", &indexList[0][0], &indexList[0][1], &indexList[0][2]);
				sscanf(y, "%d/ %d/ %d/", &indexList[1][0], &indexList[1][1], &indexList[1][2]);
				sscanf(z, "%d/ %d/ %d/", &indexList[2][0], &indexList[2][1], &indexList[2][2]);

				//		           Vertex(VectorMath3(pos),	      VectorMath4(color),  VectorMath2(Textures),  VectorMath3(normal))
				vertices.push_back(Vertex(coords[(indexList[0][0]) - 1], V4(1, 1, 1, 1), texels[(indexList[0][1]) - 1], normals[(indexList[0][2]) - 1]));
				vertices.push_back(Vertex(coords[(indexList[1][0]) - 1], V4(1, 1, 1, 1), texels[(indexList[1][1]) - 1], normals[(indexList[1][2]) - 1]));
				vertices.push_back(Vertex(coords[(indexList[2][0]) - 1], V4(1, 1, 1, 1), texels[(indexList[2][1]) - 1], normals[(indexList[2][2]) - 1]));

				indices.push_back(verticesLength);
				indices.push_back(verticesLength + 1);
				indices.push_back(verticesLength + 2);

				verticesLength += 3;

				if (reads == 3) {
					break;
				}
				// Triangulate quad
				else if (w[0] != 'f' && w[0] != '#') {

					sscanf(w, "%d/ %d/ %d/", &indexList[3][0], &indexList[3][1], &indexList[3][2]);

					vertices.push_back(Vertex(coords[(indexList[3][0]) - 1], V4(1, 1, 1, 1), texels[(indexList[3][1]) - 1], normals[(indexList[3][2]) - 1]));

					indices.push_back(verticesLength - 3);
					indices.push_back(verticesLength - 1);
					indices.push_back(verticesLength);

					verticesLength += 1;
				}
				// if accidentally read too long
				else if (w[0] == 'f') {
					buf[0] = 'f', buf[1] == '\0';
					indexCheck = false;
				}
			}
		}
		fclose(myfile);

		return std::make_shared<MeshResource>(&vertices[0], &indices[0], vertices.size(), indices.size());
	}
	else { std::cout << "Unable to open file"; }
	return NULL;
}

void MeshResource::Destroy()
{
	glDeleteBuffers(1, &this->indexBuffer);
	glDeleteBuffers(1, &this->vertexBuffer);
}

MeshResource::~MeshResource()
{
	Destroy();
}