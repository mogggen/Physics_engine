#include "config.h"
#include "render/MeshResource.h"
#include <stdio.h>
#include <inttypes.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <render/tiny_gltf.h>


MeshResource::MeshResource(Vertex vertices[], uint32_t Verticeslength, uint32_t indices[], uint32_t indicesLength) : indices(indicesLength)
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
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 7));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(sizeof(GLfloat) * 9));

	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::shared_ptr<MeshResource> MeshResource::LoadObj(const char *pathToFile)
{
	char buf[1024];
	FILE *fs;
#ifndef __linux
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
				if (fscanf(fs, "%f %f %f", &nextCoordinate.x, &nextCoordinate.y, &nextCoordinate.z) == 3)
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
				if (fscanf(fs, "%f %f", &nextTexel.x, &nextTexel.y) == 2)
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
				if (fscanf(fs, "%f %f %f", &nextNormal.x, &nextNormal.y, &nextNormal.z) == 3)
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
				uint8_t argc = fscanf(fs, "%s %s %s %s", &pos[0], &pos[1], &pos[2], &pos[3]);

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

					float dist1 = (vertices[vertices.size() - 4].pos - vertices[vertices.size() - 2].pos).Length();
					float dist2 = (vertices[vertices.size() - 3].pos - vertices[vertices.size() - 1].pos).Length();
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
	}
	else
	{
		printf("file not found with path \"./%s\"\n", pathToFile);
	}
	if (fs)
		fclose(fs);
	printf("loaded %s\n", pathToFile);
	return std::make_shared<MeshResource>(&vertices[0], vertices.size(), &indices[0], indices.size());
}



std::shared_ptr<MeshResource> MeshResource::LoadGLTF(const std::string& filePath)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err, warn;

	bool result = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);

	if (!warn.empty()) {
		// Handle warnings
	}

	if (!err.empty()) {
		// Handle errors
	}

	if (!result) {
		return nullptr;
	}

	std::vector<unsigned char> binaryData;
	std::string mime_type = "";
	bool succeded = tinygltf::DecodeDataURI(&binaryData, mime_type, model.buffers[0].uri, model.buffers[0].uri.length(), false);
	if (!succeded) return std::shared_ptr<MeshResource>();

	// scenic route
	for (const tinygltf::Scene& scene : model.scenes)
	{
		//whatever
		scene.nodes;
	}

	for (const tinygltf::Camera& camera : model.cameras)
	{
		if (camera.type == "perspective")
		{
			const tinygltf::PerspectiveCamera& per = camera.perspective;
			per.yfov;
			per.aspectRatio;
			per.znear;
			per.zfar;
		}
	}

	// Assuming you've already loaded the GLTF model and extracted binary data
	// ...

	// Find the "Cube" mesh and its primitive
	int cubeMeshIndex = 0; // Replace with the correct index for your "Cube" mesh
	int primitiveIndex = 0; // Assuming there's only one primitive for simplicity

	if (cubeMeshIndex < model.meshes.size()) {
		const tinygltf::Mesh& cubeMesh = model.meshes[cubeMeshIndex];

		if (primitiveIndex < cubeMesh.primitives.size()) {
			const tinygltf::Primitive& primitive = cubeMesh.primitives[primitiveIndex];

			// Access the accessor for position data
			int positionAccessorIndex = primitive.attributes.at("POSITION");
			const tinygltf::Accessor& positionAccessor = model.accessors[positionAccessorIndex];
			const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
			const unsigned char* positionData = &binaryData[positionBufferView.byteOffset];

			// Access the accessor for normals
			int normalAccessorIndex = primitive.attributes.at("NORMAL");
			const tinygltf::Accessor& normalAccessor = model.accessors[normalAccessorIndex];
			const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
			const unsigned char* normalData = &binaryData[normalBufferView.byteOffset];

			// Access the accessor for normals
			int texelAccessorIndex = primitive.attributes.at("TEXCOORD_0");
			const tinygltf::Accessor& texelAccessor = model.accessors[texelAccessorIndex];
			const tinygltf::BufferView& texelBufferView = model.bufferViews[texelAccessor.bufferView];
			const unsigned char* texelData = &binaryData[texelBufferView.byteOffset];


			// Access the accessor for indices (triangles)
			int indicesAccessorIndex = primitive.indices;
			if (primitive.mode == TINYGLTF_MODE_TRIANGLES)
			{
			}
			const tinygltf::Accessor& indicesAccessor = model.accessors[indicesAccessorIndex];
			const tinygltf::BufferView& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
			const unsigned char* indicesData = &binaryData[indicesBufferView.byteOffset];

			// Create lists to store vertices and indices

			std::vector<Vertex> resVert; // complete
			
			std::vector<float> tempPositionBuffer;
			std::vector<float> tempNormalBuffer; 
			std::vector<float> tempTexelBuffer;

			std::vector<V3> position3;
			std::vector<V3> normal3;
			std::vector<V2> texel2;

			std::vector<unsigned int> tri_indices; // Store triangle indices

			// Iterate through position data and convert to vertices
			for (size_t i = 0; i < positionAccessor.count; ++i) {
				const void* positionPtr = positionData + i * positionAccessor.ByteStride(positionBufferView);

				// Assuming positions are 3D (x, y, z)
				const float* position = reinterpret_cast<const float*>(positionPtr);
				for (int j = 0; j < 3; ++j) {
					tempPositionBuffer.push_back(position[j]);
				}
			}

			for (size_t i = 0; i < normalAccessor.count; ++i) {
				const void* normalPtr = normalData + i * normalAccessor.ByteStride(normalBufferView);

				// Assuming normals are 3D (x, y, z)
				const float* normal = reinterpret_cast<const float*>(normalPtr);
				for (int j = 0; j < 3; ++j) {
					tempNormalBuffer.push_back(normal[j]);
				}
			}

			for (size_t i = 0; i < texelAccessor.count; ++i) {
				const void* texelPtr = texelData + i * texelAccessor.ByteStride(texelBufferView);

				// Assuming texel are 2D (u, v)
				const float* texel = reinterpret_cast<const float*>(texelPtr);
				for (int j = 0; j < 2; ++j) {
					tempTexelBuffer.push_back(texel[j]);
				}
			}

			for (size_t i = 0; i < tempPositionBuffer.size(); i += 3)
			{
				position3.push_back(V3(
					tempPositionBuffer[i + 0],
					tempPositionBuffer[i + 1],
					tempPositionBuffer[i + 2]
				));
			}

			for (size_t i = 0; i < tempNormalBuffer.size(); i += 3)
			{
				normal3.push_back(V3(
					tempNormalBuffer[i + 0],
					tempNormalBuffer[i + 1],
					tempNormalBuffer[i + 2]
				));
			}

			for (size_t i = 0; i < tempTexelBuffer.size(); i += 2)
			{
				texel2.push_back(V2(
					tempTexelBuffer[i + 0],
					tempTexelBuffer[i + 1]
				));
			}

			assert(texel2.size() == normal3.size() && normal3.size() == position3.size());
			for (size_t k = 0; k < position3.size(); ++k)
			{
				resVert.push_back(
					{
						position3[k],
						V4(1, 1, 1, 1),
						texel2[k],
						normal3[k],
					});
			}

			// Iterate through indices data and convert to indices
			for (size_t i = 0; i < indicesAccessor.count; ++i) {
				const void* indexPtr = indicesData + i * indicesAccessor.ByteStride(indicesBufferView);
				if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					// Assuming indices are UNSIGNED_SHORT
					const unsigned short* index = reinterpret_cast<const unsigned short*>(indexPtr);
					//if (index[0] == 65021) tri_indices.push_back(1);
					/*else */tri_indices.push_back(index[0]);
					//if (index[1] == 65021) tri_indices.push_back(1);
					/*else */tri_indices.push_back(index[1]);
					//if (index[2] == 65021) tri_indices.push_back(1);
					/*else */tri_indices.push_back(index[2]);
				}
				else if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					const unsigned int* index = reinterpret_cast<const unsigned int*>(indexPtr);
					tri_indices.push_back(index[0]);
					tri_indices.push_back(index[1]);
					tri_indices.push_back(index[2]);
				}
			}

			/*std::map<int, int> indexMap;
			std::vector<Vertex> uniqueVertices;
			std::vector<unsigned int> uniqueIndices;
			
			for (unsigned int i = 0; i < tri_indices.size(); ++i) {
				int vertexIndex = tri_indices[i];
				if (indexMap.find(vertexIndex) == indexMap.end()) {
					indexMap[vertexIndex] = uniqueVertices.size();
					uniqueVertices.push_back(resVert[vertexIndex]);
				}
				uniqueIndices.push_back(indexMap[vertexIndex]);
			}*/

			
			//for (size_t ss = 0; ss < tri_indices.size(); ss++)
			//{
			//	auto first = std::find(tri_indices.begin(), tri_indices.end(), tri_indices[ss]);
			//	auto pos = std::find(first + 1, tri_indices.end(), tri_indices[ss]);
			//	if (pos == tri_indices.end()) continue;

			//	tri_indices.erase(pos);
			//	ss = 0;
			//}
			

			// Now, you have the vertices and indices ready for rendering
			return std::make_shared<MeshResource>(&resVert[0], resVert.size(), &tri_indices[0], tri_indices.size());
		}
	}
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