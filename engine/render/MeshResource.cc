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



std::shared_ptr<MeshResource> MeshResource::Cube()
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
				V3(0, 1, 0)
			},
			Vertex // left : 1
			{
				V3(-.5f, -.5f, -.5f),
				left,
				V2(0, 1),
				V3(0, 0, 0)},
			Vertex // bottom : 2
			{
				V3(-.5f, -.5f, -.5f),
				bottom,
				V2(1, 0),
				V3(0, 0, 0)},

			// 100
			Vertex // back : 3
			{
				V3(.5f, -.5f, -.5f),
				back,
				V2(0, 1),
				V3(0, 0, 0)},
			Vertex // right : 4
			{
				V3(.5f, -.5f, -.5f),
				right,
				V2(1, 1),
				V3(0, 0, 0)},
			Vertex // bottom : 5
			{
				V3(.5f, -.5f, -.5f),
				bottom,
				V2(0, 0),
				V3(0, 0, 0)},

			// 010
			Vertex // back : 6
			{
				V3(-.5f, .5f, -.5f),
				back,
				V2(1, 0),
				V3(0, 0, 0)},
			Vertex // left : 7
			{
				V3(-.5f, .5f, -.5f),
				left,
				V2(0, 0),
				V3(0, 0, 0)},
			Vertex // top : 8
			{
				V3(-.5f, .5f, -.5f),
				top,
				V2(0, 0),
				V3(0, 0, 0)},

			// 110
			Vertex // back : 9
			{
				V3(.5f, .5f, -.5f),
				back,
				V2(0, 0),
				V3(0, 0, 0)},
			Vertex // right : 10
			{
				V3(.5f, .5f, -.5f),
				right,
				V2(1, 0),
				V3(0, 0, 0)},
			Vertex // top : 11
			{
				V3(.5f, .5f, -.5f),
				top,
				V2(1, 0),
				V3(0, 0, 0)},

			// 001
			Vertex // left : 12
			{
				V3(-.5f, -.5f, .5f),
				left,
				V2(1, 1),
				V3(0, 0, 0)},
			Vertex // front : 13
			{
				V3(-.5f, -.5f, .5f),
				front,
				V2(0, 1),
				V3(0, 0, 0)},
			Vertex // bottom : 14
			{
				V3(-.5f, -.5f, .5f),
				bottom,
				V2(1, 1),
				V3(0, 0, 0)},

			// 101
			Vertex // right : 15
			{
				V3(.5f, -.5f, .5f),
				right,
				V2(0, 1),
				V3(0, 0, 0)},
			Vertex // front : 16
			{
				V3(.5f, -.5f, .5f),
				front,
				V2(1, 1),
				V3(0, 0, 0)},
			Vertex // bottom : 17
			{
				V3(.5f, -.5f, .5f),
				bottom,
				V2(0, 1),
				V3(0, 0, 0)},

			// 011
			Vertex // left : 18
			{
				V3(-.5f, .5f, .5f),
				left,
				V2(1, 0),
				V3(0, 0, 0)},
			Vertex // front : 19
			{
				V3(-.5f, .5f, .5f),
				front,
				V2(0, 0),
				V3(0, 0, 0)},
			Vertex // top : 20
			{
				V3(-.5f, .5f, .5f),
				top,
				V2(0, 1),
				V3(0, 0, 0)},

			// 111
			Vertex // right : 21
			{
				V3(.5f, .5f, .5f),
				right,
				V2(0, 0),
				V3(0, 0, 0)},
			Vertex // front : 22
			{
				V3(.5f, .5f, .5f),
				front,
				V2(1, 0),
				V3(0, 0, 0)},
			Vertex // top : 23
			{
				V3(.5f, .5f, .5f),
				top,
				V2(1, 1),
				V3(0, 0, 0)},
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


std::shared_ptr<MeshResource> LoadObj(const char *pathToFile,
		std::vector<uint32>& _indices,
		std::vector<V3>& _positions,
		std::vector<V2>& _texels,
		std::vector<V3>& _normals,
		std::vector<Vertex>& _vertices)
{
	char buf[1024];
	FILE *fs;
#ifndef __linux__
	fopen_s(&fs, pathToFile, "r"); // textures/sphere.obj
#else
	fs = fopen64(pathToFile, "r"); // "textures/sphere.obj"
#endif

	unsigned long long verticesUsed = 0ull;
	std::vector<uint32_t> indices;
	std::vector<V3> coords;
	std::vector<V2> texels;
	std::vector<V3> normals;
	std::vector<Vertex> vertices; // complete package

	if (fs)
	{
		while (true)
		{
			int foo = fscanf(fs, "%1024s", buf); // reads one word at a time and places it at buf[0] with a trailing '\0'
			if (foo <= 0)
			{
				break;
			}

			if (buf[0] == 'v' && buf[1] == '\0')
			{
				V3 nextCoordinate;
				if (fscanf(fs, "%f %f %f", &nextCoordinate[0], &nextCoordinate[1], &nextCoordinate[2]) == 3)
				{
					coords.push_back(nextCoordinate);
				}
				else
				{
					std::cerr << "missing arguments in vertex, expected 3" << std::endl;
				}
			}

			else if (buf[0] == 'v' && buf[1] == 't' && buf[2] == '\0')
			{
				V2 nextTexel;
				if (fscanf(fs, "%f %f", &nextTexel[0], &nextTexel[1]) == 2)
				{
					texels.push_back(nextTexel);
				}
				else
				{
					std::cerr << "missing arguments in texel, expected 2" << std::endl;
				}
			}

			else if (buf[0] == 'v' && buf[1] == 'n' && buf[2] == '\0')
			{
				V3 nextNormal;
				if (fscanf(fs, "%f %f %f", &nextNormal[0], &nextNormal[1], &nextNormal[2]) == 3)
				{
					normals.push_back(nextNormal);
				}
				else
				{
					std::cerr << "missing arguments in texel, expected 3" << std::endl;
				}
			}

			else if (buf[0] == 'f' && buf[1] == '\0')
			{
				char pos[4][64];
				uint8_t argc = fscanf(fs, "%s %s %s"/*"%s"*/, &pos[0], &pos[1], &pos[2]/*, &pos[3]*/);

				uint32_t listOfIndices[4][3];

				if (argc == 4 && pos[3][0] != 'f' && pos[3][0] != '#')
				{
					for (size_t i = 0; i < 4; i++)
					{
						if (sscanf(pos[i], "%lu"
										   "/ %lu"
										   "/ %lu"
										   "/",
								   &listOfIndices[i][0], &listOfIndices[i][1], &listOfIndices[i][2]) != 3)
							break;
							vertices.push_back(Vertex{
								coords[(listOfIndices[i][0]) - 1],
								V4(1, 1, 1, 1),
								texels[(listOfIndices[i][1]) - 1],
								normals[(listOfIndices[i][2]) - 1],
							});
					}

					float dist1 = (vertices[vertices.size() - 4].pos - vertices[vertices.size() - 2].pos).Length2();
					float dist2 = (vertices[vertices.size() - 3].pos - vertices[vertices.size() - 1].pos).Length2();
					if (dist1 > dist2)
					{
						indices.push_back(vertices.size() - 4);
						indices.push_back(vertices.size() - 3);
						indices.push_back(vertices.size() - 1);

						indices.push_back(vertices.size() - 3);
						indices.push_back(vertices.size() - 2);
						indices.push_back(vertices.size() - 1);
					}
					else
					{
						indices.push_back(vertices.size() - 4);
						indices.push_back(vertices.size() - 3);
						indices.push_back(vertices.size() - 2);

						indices.push_back(vertices.size() - 4);
						indices.push_back(vertices.size() - 2);
						indices.push_back(vertices.size() - 1);
					}
				}
				else if (argc == 3)
				{
					for (size_t i = 0; i < 3; i++)
					{
						if (sscanf(pos[i], "%lu"
										   "/ %lu"
										   "/ %lu"
										   "/",
								   &listOfIndices[i][0], &listOfIndices[i][1], &listOfIndices[i][2]) != 3)
								   break;

							vertices.push_back(Vertex{
								coords[listOfIndices[i][0] - 1],
								V4(1, 1, 1, 1),
								texels[listOfIndices[i][1] - 1],
								normals[listOfIndices[i][2] - 1],
							});
						indices.push_back(vertices.size() - 1);
					}
				}
			}
		}

		for (unsigned const& var : indices)
		{
			_indices.push_back(var);
		}

		for (V3 const& var : coords)
		{
			_positions.push_back(var);
		}

		for (V2 const& var : texels)
		{
			_texels.push_back(var);
		}

		for (V3 const& var : normals)
		{
			_normals.push_back(var);
		}

		for (Vertex const& var : vertices)
		{
			_vertices.push_back(var);
		}

	}
	else
	{
		printf("file not found with path \"./%s\"\n", pathToFile);
	}
	if (fs != nullptr)
	{
		fclose(fs);
	}
	printf("loaded %s\n", pathToFile);
	return std::make_shared<MeshResource>(&vertices[0], vertices.size(), &indices[0], indices.size());
}

std::shared_ptr<MeshResource> MeshResource::LoadObj(const char* pathToFile, std::vector<uint32>& _indices,
	std::vector<V3>& _positions,
	std::vector<V2>& _texels,
	std::vector<V3>& _normals,
	std::vector<Vertex>& _vertices)
{
	if (!_vertices.empty()) _vertices.clear();
	if (!_indices.empty()) _indices.clear();

	using namespace std;
	ifstream fs(pathToFile);
	string lineRemainder;

	if (!fs)
	{
		printf("file not found with path \"./%s\"\n", pathToFile);
		return nullptr;
	}

	unsigned long long verticesUsed = 0ull;
	vector<V3> coords;
	vector<V2> texels;
	vector<V3> normals;

	while (getline(fs, lineRemainder))
	{
		size_t pos = 0;
		string token;

		if ((pos = lineRemainder.find(" ")) != string::npos) {
			token = lineRemainder.substr(0, pos);
			lineRemainder.erase(0, pos + 1);
			if (token == "v")
			{
				size_t I = 0;
				V3 nextCoordinate;

				while ((pos = lineRemainder.find(" ")) != string::npos)
				{
					if (I >= 3)
					{
						cerr << "too many arguments in vertex, expected 3" << endl;
						break;
					}
					token = lineRemainder.substr(0, pos);
					lineRemainder.erase(0, pos + 1);
					nextCoordinate[I++] = stof(token);
				}
				nextCoordinate[I++] = stof(lineRemainder);
				if (I < 3U) cerr << "not enough love, vertex";
				coords.push_back(nextCoordinate);
			}

			else if (token == "vt")
			{
				size_t I = 0;
				V2 nextTexel;

				while ((pos = lineRemainder.find(" ")) != string::npos)
				{
					if (I >= 2)
					{
						cerr << "too many arguments in texel, expected 2" << endl;
						break;
					}
					token = lineRemainder.substr(0, pos);
					lineRemainder.erase(0, pos + 1);
					nextTexel[I++] = stof(token);
				}
				nextTexel[I++] = stof(lineRemainder);
				if (I < 2)
				{
					cerr << "not enough love, texel";
					break;
				}
				texels.push_back(nextTexel);
			}

			else if (token == "vn")
			{
				size_t I = 0;
				V3 nextNormal;

				while ((pos = lineRemainder.find(" ")) != string::npos)
				{
					if (I >= 3)
					{
						cerr << "missing arguments in normal, expected 3" << endl;
						break;
					}
					token = lineRemainder.substr(0, pos);
					lineRemainder.erase(0, pos + 1);
					nextNormal[I++] = stof(token);
				}
				nextNormal[I++] = stof(lineRemainder);
				if (I < 3U)
				{
					cerr << "not enough love, normals";
					break;
				}
				normals.push_back(nextNormal);
			}

			else if (token == "f")
			{
				size_t I = 0;
				vector<string> args;

				while ((pos = lineRemainder.find(" ")) != string::npos)
				{
					if (I >= 5)
					{
						cerr << "missing arguments in face, expected 3 or 4" << endl;
						break;
					}
					token = lineRemainder.substr(0, pos);
					lineRemainder.erase(0, pos + 1);
					args.push_back(token);
				}
				args.push_back(lineRemainder);
				//for (string s : args) cout << s << endl;


				size_t posi = 0;
				string tokenSmall;
				vector<unsigned> argi;

				for (size_t i = 0; i < args.size(); i++)
				{
					argi.clear();
					while ((posi = args[i].find("/")) != string::npos)
					{
						if (argi.size() >= 4)
						{
							cout << "i: " << i << endl;
							cout << "argi: ";
							for (auto s : argi) cout << s << " ";
							cout << endl;
							cerr << "too many arguments in faceProperties, expected 2 or 3" << endl;
							break;
						}
						tokenSmall = args[i].substr(0, posi);
						args[i].erase(0, posi + 1);
						argi.push_back(stoi(tokenSmall));
					}
					argi.push_back(stoi(args[i]));
					if (argi.size() < 2)
					{
						cerr << "not enough parameters in faceProperies 2 or 3";
						break;
					}

					_vertices.push_back(Vertex{
						coords[(argi[0]) - 1],
						V4(1, 1, 1, 1),
						texels[(argi[1]) - 1],
						(argi.size() == 3 ? normals[argi[2] - 1] : V3()),
						});
					if (args.size() == 3)
						_indices.push_back(argi[0] - 1);
				}
				if (args.size() == 4)
				{
					float dist1 = (_vertices[_vertices.size() - 4].pos - _vertices[_vertices.size() - 2].pos).Length();
					float dist2 = (_vertices[_vertices.size() - 3].pos - _vertices[_vertices.size() - 1].pos).Length();
					if (dist1 > dist2)
					{
						_indices.push_back(_vertices.size() - 4);
						_indices.push_back(_vertices.size() - 3);
						_indices.push_back(_vertices.size() - 1);

						_indices.push_back(_vertices.size() - 3);
						_indices.push_back(_vertices.size() - 2);
						_indices.push_back(_vertices.size() - 1);
					}
					else
					{
						_indices.push_back(_vertices.size() - 4);
						_indices.push_back(_vertices.size() - 3);
						_indices.push_back(_vertices.size() - 2);

						_indices.push_back(_vertices.size() - 4);
						_indices.push_back(_vertices.size() - 2);
						_indices.push_back(_vertices.size() - 1);
					}
				}
			}
		}
	}

	for (V3 const& var : coords)
	{
		_positions.push_back(var);
	}

	for (V2 const& var : texels)
	{
		_texels.push_back(var);
	}

	for (V3 const& var : normals)
	{
		_normals.push_back(var);
	}

	
	printf("loadedToBuffer %s\n", pathToFile);
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