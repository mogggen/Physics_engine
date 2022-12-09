//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "imgui.h"
#include "stb_image.h"
#include "exampleapp.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <chrono>
#include <limits>
#ifdef __linux__
#include <unistd.h>
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "render/stb_image_write.h"
struct Actor;

using namespace Display;
namespace Example
{
	//------------------------------------------------------------------------------
	/**
	 */
	ExampleApp::ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	 */
	ExampleApp::~ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	 */

	class SoftwareRenderer
	{
		GLuint vertexBufHandle;
		GLuint indexBufHandle;
		// get from foo.obj
		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;
		std::vector<float> depthBuffer; // one depth for each pixel, loop raster
		int pixelCounter = 0;
		// obj depth buffer


		//resulting frame
		unsigned char* frameBuffer = nullptr;
		int rasterWidth, rasterHeight;
		
		// the texture
		unsigned char* textureBuffer = nullptr;
		int textureWidth, textureHeight;
		
		// the openGL texture size
	public:
		int widthImg = 1920, heightImg = 1020;
		Camera cam;
		Lightning light;

		SoftwareRenderer();
		SoftwareRenderer(unsigned rzw, unsigned rzh);
		void setVertexBuffer(std::vector<Vertex> vertexBuffer);
		std::vector<Vertex>& getVertexBuffer();
		std::vector<unsigned>& getIndexBuffer();
		std::vector<float>& getDepthBuffer();
		void setIndexBuffer(unsigned* indexBuffer);
		unsigned char* getFrameBuffer();
		void setFrameBuffer(unsigned char* frameBuffer);
		void triangle(vec2 a, vec2 b, vec2 c, const vec2& ta, const vec2& tb, const vec2& tc);
		void append_line(int x0, int y0, int x1, int y1, vec3 color);
		bool DrawToDepthBuffer(unsigned x, unsigned y, const float depth);
		float getDepthFromPixel(const vec2& p, const vec3& a, const vec3& b, const vec3& c);
		void moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec2& norm);
		void moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec3& norm);
		void projectModel(const mat4& pvm);
		vec3 RasterizeLight(const vec3& lightSourceColor, const vec3& TextureRGB, const vec3& fragPosOut, const vec3& normalOut);
		void loadTexture(const char* pathToFile);
		bool LoadObj(const char* pathToFile, std::vector<Vertex>& vertices, std::vector<unsigned>& indices, float* _coordinates, unsigned* _indices, float* _texels, float* _normals);
		bool LoadObj(const char* pathToFile, std::vector<Vertex>& vertices, std::vector<unsigned>& indices);
		int saveRender(int w = 1920, int h = 1200, const char* pathToFile="textures/res.jpg");

		unsigned char* getTextureBuffer();
		int getTextureWidth();
		int getTextureHeight();


		void projectVertex(vec3& v);
		bool isInTriangle(const vec2& p, const vec2& a, const vec2& b, const vec2& c);
		float sign(const vec2& p, const vec2& a, const vec2& b);
		vec3 getBarycentricCoord(const vec2& p, const vec2& a, const vec2& b, const vec2& c);
		vec2 UVmapping(const vec3& bp, const vec2& ta, const vec2& tb, const vec2& tc);
		vec3 getPixelColorFromUV(const unsigned char* texture, const int& w, const int& h, vec2 texel);
		void setBackground(const vec3& color=vec3(0.0f, 0.0f, 0.0f));
		void rasterizeTriangle(vec3 ca, vec3 cb, vec3 cc, const vec2& ta, const vec2& tb, const vec2& tc);
	};

	SoftwareRenderer::SoftwareRenderer()
	{
		std::cerr << "CAUTION: widthImg and heightImg is undefined" << std::endl;
	}

	SoftwareRenderer::SoftwareRenderer(unsigned razWidth, unsigned razHeight) : widthImg(razWidth), heightImg(razHeight)
	{
		stbi_set_flip_vertically_on_load(true);
		stbi_flip_vertically_on_write(true);

		depthBuffer.resize(widthImg * heightImg);
		for (size_t i = 0; i < widthImg * heightImg; i++)
		{
			depthBuffer[i] = std::numeric_limits<float>::lowest();
		}

		frameBuffer = new unsigned char[widthImg * heightImg * 3];

		cam = Camera(90, (float)widthImg / heightImg, 0.01f, 100.0f);
		cam.setPos(vec4(0, 2, 5));
		cam.setRot(vec4(0, 1, 0), M_PI);

		light = Lightning(vec3(10, 10, 10), vec3(1, 1, 1), .01f);
	}

	std::vector<Vertex>& SoftwareRenderer::getVertexBuffer()
	{
		return vertices;
	}

	std::vector<unsigned>& SoftwareRenderer::getIndexBuffer()
	{
		return indices;
	}

	void SoftwareRenderer::setFrameBuffer(unsigned char* _frameBuffer)
	{
		if (frameBuffer)
			delete[] frameBuffer;
		frameBuffer = _frameBuffer;
	}

	vec2 modelToRes(vec3 coord, const unsigned widthImg, const unsigned heightImg)
	{
		return vec2(coord.x * 0.75 * widthImg, coord.y * 0.75 * heightImg);
	}

	void SoftwareRenderer::append_line(int x0, int y0, int x1, int y1, vec3 color)
	{
		bool steep = false;
		x0 *= 3;
		y0 *= 3;
		x1 *= 3;
		y1 *= 3;

		if (std::abs(x0 - x1) < std::abs(y0 - y1))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			steep = true;
		}
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
		int dx = x1 - x0;
		int dy = y1 - y0;
		int derror2 = std::abs(dy) * 2;
		int error2 = 0;
		int y = y0;
		const int yincr = -1 + 2 * (y1 > y0);
		if (steep)
		{
			for (int x = x0; x <= x1 && x + widthImg * 3 * y >= 0 && x + widthImg * 3 * y < widthImg * 3 * heightImg; x += 3)
			{
				frameBuffer[x + widthImg * 3 * y] = color.r;
				frameBuffer[x + widthImg * 3 * y + 1] = color.g;
				frameBuffer[x + widthImg * 3 * y + 2] = color.b;
				//img.set_pixel_color(y, x, color);
				error2 += derror2;
				if (error2 > dx)
				{
					y += yincr;
					error2 -= dx * 2;
				}
			}
		}
		else
		{
			for (int x = x0; x <= x1 && y + widthImg * 3 * x >= 0 && y + widthImg * 3 * x < widthImg * 3 * heightImg; x += 3)
			{
				frameBuffer[y + widthImg * 3 * x] = color.r;
				frameBuffer[y + widthImg * 3 * x + 1] = color.g;
				frameBuffer[y + widthImg * 3 * x + 2] = color.b;
				//img.set_pixel_color(x, y, color);
				error2 += derror2;
				if (error2 > dx) {
					y += yincr;
					error2 -= dx * 2;
				}
			}
		}
	}

	void Print(mat4 m);

	inline void SoftwareRenderer::projectVertex(vec3& v)
	{
		const mat4 start = Translate(vec4(v));
		const mat4 projection = cam.pv() * start;
		/*Print(start);
		Print(projection);*/
		//printf("%f %f %f", projection[0][3], projection[1][3], projection[2][3]);
		v = vec3(projection[0][3], projection[1][3], projection[2][3]) / projection[3][3];
	}

	inline float SoftwareRenderer::sign(const vec2& p, const vec2& a, const vec2& b)
	{
		return (p.x - b.x) * (a.y - b.y) - (p.y - b.y) * (a.x - b.x);
	}

	inline bool SoftwareRenderer::isInTriangle(const vec2& p, const vec2& a, const vec2& b, const vec2& c)
	{
		float d1, d2, d3;
		bool has_neg, has_pos;

		d1 = sign(p, a, b);
		d2 = sign(p, b, c);
		d3 = sign(p, c, a);

		has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		if (!has_neg || !has_pos) pixelCounter++;
		return !has_neg || !has_pos;
	}

	const float safeDivide(const float& lhs, const float& rhs, const float result=0)
	{
		return rhs != 0 ? lhs / rhs : result;
	}

	vec3 SoftwareRenderer::getBarycentricCoord(const vec2& p, const vec2& a, const vec2& b, const vec2& c)
	{
		float L1 = safeDivide((p.y - c.y) * (b.x - c.x) + (p.x - c.x) * (c.y - b.y),
			(a.y - c.y) * (b.x - c.x) + (a.x - c.x) * (c.y - b.y), 1);

		float L2 = safeDivide(p.x - L1 * a.x - c.x + L1 * c.x, b.x - c.x, 1);

		float L3 = 1 - L1 - L2;

		return vec3(L1, L2, L3);
	}

	// resulting barycentric coordinate p
	// texel coordinates for a-c
	inline vec2 SoftwareRenderer::UVmapping(const vec3& bp, const vec2& ta, const vec2& tb, const vec2& tc)
	{
		return vec2(ta * bp.x + tb * bp.y + tc * bp.z);
	}

	inline float SoftwareRenderer::getDepthFromPixel(const vec2& p, const vec3& a, const vec3& b, const vec3& c)
	{
		vec3 norm(Cross(a - b, a - c));
		return a.z - ((p.x - a.x) * norm.x + (p.y - a.y) * norm.y) / norm.z;
	}

	inline vec3 SoftwareRenderer::getPixelColorFromUV(const unsigned char *texture, const int& w, const int& h, vec2 texel)
	{
		// GL_REPEAT
		while (texel.u > 1.f) texel.u -= 1.f;
		while (texel.v > 1.f) texel.v -= 1.f;
		while (texel.u < 0.f) texel.u += 1.f;
		while (texel.v < 0.f) texel.u += 1.f;

		int pixelX = (texel.u) * w;
		int pixelY = (texel.v) * h;
		int index = (pixelX + pixelY * w) * 3;
		if (index + 2 < w * h * 3)
			return vec3(texture[index], texture[index + 1], texture[index + 2]);
		return vec3(-1, -1, -1);
	}

	inline void SoftwareRenderer::loadTexture(const char* pathToFile)
	{
		int nrChannels;
		textureBuffer = stbi_load(pathToFile, &textureWidth, &textureHeight, &nrChannels, 3);
		if (!textureBuffer)
		{
			fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", pathToFile, stbi_failure_reason());
			exit(0);
		}
	}

	inline bool SoftwareRenderer::LoadObj(const char* pathToFile, std::vector<Vertex>& vertices, std::vector<unsigned>& indices, float* _coordinates, unsigned* _indices, float* _texels, float* _normals)
	{
		char wordBuf[1024];
		FILE* fs;
#ifndef __linux
		fopen_s(&fs, pathToFile, "r"); // textures/sphere.obj
#else
		fs = fopen64(pathToFile, "r"); // "textures/sphere.obj"
#endif

		setBackground(vec3(0x69, 0x69, 0x69));
		unsigned long long verticesUsed = 0ull;
		std::vector<vec3> coords;
		std::vector<vec2> texels;
		std::vector<vec3> normals;

		if (fs)
		{
			while (true) // reads one word at a time and places it at wordBuf[0] with a trailing '\0'
			{
				int count = fscanf(fs, "%1024s", wordBuf);
				if (count <= 0)
				{
					break;
				}
				if (wordBuf[0] == 'v' && wordBuf[1] == '\0')
				{
					vec3 nextCoordinate;
					if (fscanf(fs, "%f %f %f", &nextCoordinate.x, &nextCoordinate.y, &nextCoordinate.z) == 3)
					{
						coords.push_back(nextCoordinate);
					}
					else
					{
						std::cerr << "missing arguments in vertex, expected 3" << std::endl;
					}
				}

				else if (wordBuf[0] == 'v' && wordBuf[1] == 't' && wordBuf[2] == '\0')
				{
					vec2 nextTexel;
					if (fscanf(fs, "%f %f", &nextTexel.x, &nextTexel.y) == 2)
					{
						texels.push_back(nextTexel);
					}
					else
					{
						std::cerr << "missing arguments in texel, expected 2" << std::endl;
					}
				}

				else if (wordBuf[0] == 'v' && wordBuf[1] == 'n' && wordBuf[2] == '\0')
				{
					vec3 nextNormal;
					if (fscanf(fs, "%f %f %f", &nextNormal.x, &nextNormal.y, &nextNormal.z) == 3)
					{
						normals.push_back(nextNormal);
					}
					else
					{
						std::cerr << "missing arguments in texel, expected 3" << std::endl;
					}
				}

				else if (wordBuf[0] == 'f' && wordBuf[1] == '\0')
				{
					char pos[4][64];
					uint8_t argc = fscanf(fs, "%s %s %s"/* % s"*/, &pos[0], &pos[1], &pos[2]/*, &pos[3]*/);

					uint32_t listOfIndices[4][3];

					if (argc == 4 && pos[3][0] != 'f' && pos[3][0] != '#')
					{
						for (size_t i = 0; i < 4; i++)
						{
							int count = sscanf(pos[i], "%lu"
								"/ %lu"
								"/ %lu"
								"/",
								&listOfIndices[i][0], &listOfIndices[i][1], &listOfIndices[i][2]);
							std::cin.get();

							if (count != 3)
								break;
							vertices.push_back(Vertex{
								coords[(listOfIndices[i][0]) - 1],
								vec4(1, 1, 1, 1),
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
								vec4(1, 1, 1, 1),
								texels[listOfIndices[i][1] - 1],
								normals[listOfIndices[i][2] - 1],
								});
							indices.push_back(vertices.size() - 1);
						}
					}
				}
			}
			fclose(fs);
		}
		else
		{
			printf("file not found with path \"./%s\"\n", pathToFile);
		}

		for (size_t i = 0; i < coords.size(); i += 3)
		{
			vec3 a = coords[indices[i]];
			vec3 b = coords[indices[i + 1]];
 			vec3 c = coords[indices[i + 2]];

			projectVertex(a);
			projectVertex(b);
			projectVertex(c);

			vec2 ta = texels[indices[i]];
			vec2 tb = texels[indices[i + 1]];
			vec2 tc = texels[indices[i + 2]];

			rasterizeTriangle(a, b, c, ta, tb, tc);
		}

		//std::cout << (coords.size() == indices.size() && coords.size() == texels.size() && coords.size() == normals.size() ? "same length" : "diffrent length");
		/*_coordinates = new float[coords.size() * 3];
		_indices = new unsigned[indices.size()];
		_texels = new float[texels.size() * 2];
		_normals = new float[normals.size() * 3];

		for (size_t i = 0; i < coords.size() * 3; i += 3)
		{
			float x = coords[i][0];
			float y = coords[i][1];
			float z = coords[i][2];
			_coordinates[i] = x;
			_coordinates[i + 1] = y;
			_coordinates[i + 2] = z;
		}

		for (size_t i = 0; i < normals.size(); i++)
		{
			float x = normals[i][0];
			float y = normals[i][1];
			float z = normals[i][2];
			_normals[i] = x;
			_normals[i + 1] = y;
			_normals[i + 2] = z;
		}

		for (size_t i = 0; i < texels.size(); i++)
		{
			float x = texels[i][0];
			float y = texels[i][1];
			_texels[i] = x;
			_texels[i + 1] = y;
		}

		for (size_t i = 0; i < indices.size(); i++)
		{
			_indices[i] = indices[i];
		}*/

		printf("step 1/5: transforming obj verticies to modelSpace\n");


		//int res = stbi_write_png("textures/res.jpg", widthImg, heightImg, 3, frameBuffer, widthImg * 3);

		//save for later

		//int centerX = widthImg / 2;
		//int centerY = heightImg / 2;
		//float angOfs = M_PI / 12;
		//float angle = M_PI / 4;

		/*append_line(centerX, centerY, 0, 0, vec3(0, 0, 0));
		append_line(centerX, centerY, widthImg, 0, vec3(255, 255, 0));
		append_line(centerX, centerY, 0, heightImg, vec3(0, 0, 255));
		append_line(centerX, centerY, widthImg, heightImg, vec3(255, 0, 0));
		append_line(600, 300, 100, 90, vec3(0, 255, 0));

		append_line(900, 900, 1920, 900, vec3(0, 0, 0));

		for (size_t i = 0; i < 9; i++)
		{
			append_line(centerX, centerY, centerX * cosf(angOfs + angle * i) * 100.f, centerY * sinf(angOfs * angle * i) * 100.f, vec3((i % 3) * 255.f, ((i + 1) % 3) * 255.f, ((i + 2) % 3) * 255.f));
		}
		saveRender(widthImg, heightImg, "textures/res10.jpg");

		for (size_t i = 0; i < false && vertices.size() / 3; i += 3)
		{
			vertices[indices[i]].pos = vertices[indices[i]].pos * 100.f;
			vertices[indices[i]].pos.x = vertices[indices[i]].pos.x + widthImg * .5f;
			vertices[indices[i + 1]].pos = vertices[indices[i + 1]].pos * 100.f;
			vertices[indices[i + 1]].pos.x = vertices[indices[i + 1]].pos.x + widthImg * .5f;
			vertices[indices[i + 2]].pos = vertices[indices[i + 2]].pos * 100.f;
			vertices[indices[i + 2]].pos.x = vertices[indices[i + 2]].pos.x + widthImg * .5f;

			append_line(vertices[indices[i]].pos.x, vertices[indices[i]].pos.y, vertices[indices[i + 1]].pos.x, vertices[indices[i + 1]].pos.y, vec3(255, 0, 0));
			append_line(vertices[indices[i + 1]].pos.x, vertices[indices[i + 1]].pos.y, vertices[indices[i + 2]].pos.x, vertices[indices[i + 2]].pos.y, vec3(0, 255, 0));
			append_line(vertices[indices[i + 2]].pos.x, vertices[indices[i + 2]].pos.y, vertices[indices[i]].pos.x, vertices[indices[i]].pos.y, vec3(0, 0, 255));
		}*/

		/*printf("populate faces...\n");
		Plane* faces = new Plane[coords.size() / 3];
		size_t lenFaces = sizeof(faces) / sizeof(*faces);

		printf("num of faces: %llu\n", lenFaces);
		for (size_t i = 0; i < lenFaces; i++)
		{
			vec3 a = vec3(coords[indices[i * 3]]);
			vec3 b = vec3(coords[indices[i * 3 + 1]]);
			vec3 c = vec3(coords[indices[i * 3 + 2]]);

			faces[i] = Plane(a, Cross(a - b, a - c));
		}*/

		//printf("calculating object...\n");


		//for (size_t y = 0; y < heightImg; y++)
		//{
		//	if (y % 100 == 0 && y != 0)
		//		printf("%f\n", y / (float)heightImg);
		//	for (size_t x = 0; x < widthImg * 3; x += 3)
		//	{
		//		for	(size_t i = 0; i < lenFaces; i += 3)
		//		{
		//			//vec2 ta = texels[indices[i]];
		//			//vec2 tb = texels[indices[i + 1]];
		//			//vec2 tc = texels[indices[i + 2]];

		//			// for every vertex check, calculate it's face plane and get the line intersection pick the depth and store which pixel is closes to the screen, no alpha support
		//			
		//			vec2 a = modelToRes(coords[indices[i]], this->widthImg, this->heightImg);
		//			vec2 b = modelToRes(coords[indices[i + 1]], this->widthImg, this->heightImg);
		//			vec2 c = modelToRes(coords[indices[i + 2]], this->widthImg, this->heightImg);
		//			vec3 coord = vec3(x, y, 0);


		//			vec3 rgb;// = this->depthPassForEachPixel(x, y, a, b, c/*, face*/);
		//			frameBuffer[x * y * widthImg] = rgb.r;
		//			frameBuffer[x * y * widthImg + 1] = rgb.g;
		//			frameBuffer[x * y * widthImg + 2] = rgb.b;
		//		}

		//	}
		//}
		////this->rasterizeTriangle(a, b, c, ta, tb, tc);

		//saveRender(widthImg, heightImg, "textures/res.jpg");
		// _indices = &indices[0];
		// _coordinates = &coords[0];
		// _texels = &texels[0];
		// _normals = &normals[0];
		return fs;
	}

	inline bool SoftwareRenderer::LoadObj(const char* pathToFile, std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
	{
		using namespace std;
		ifstream fs(pathToFile);
		string line;

		if (!fs)
		{
			printf("file not found with path \"./%s\"\n", pathToFile);
			return false;
		}

		setBackground(vec3(0x69, 0x69, 0x69));
		unsigned long long verticesUsed = 0ull;
		vector<vec3> coords;
		vector<vec2> texels;
		vector<vec3> normals;

		while (getline(fs, line))
		{
			size_t pos = 0;
			string token;

			while ((pos = line.find(" ")) != string::npos) {
				token = line.substr(0, pos);
				line.erase(0, pos + 1);
				
				if (token == "v")
				{
					size_t I = 0;
					vec3 nextCoordinate;
					
					while ((pos = line.find(" ")) != string::npos)
					{
						if (I >= 3)
						{
							cerr << "too many arguments in vertex, expected 3" << endl;
							break;
						}
						token = line.substr(0, pos);
						line.erase(0, pos + 1);
						nextCoordinate[I++] = stof(token);
					}
					nextCoordinate[I++] = stof(line);
					if (I<3U) cerr << "not enough love, vertex";
					coords.push_back(nextCoordinate);
				}

				else if (token == "vt")
				{
					size_t I = 0;
					vec2 nextTexel;

					while ((pos = line.find(" ")) != string::npos)
					{
						if (I >= 2)
						{
							cerr << "too many arguments in texel, expected 2" << endl;
							break;
						}
						token = line.substr(0, pos);
						line.erase(0, pos + 1);
						nextTexel[I++] = stof(token);
					}
					nextTexel[I++] = stof(line);
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
					vec3 nextNormal;

					while ((pos = line.find(" ")) != string::npos)
					{
						if (I >= 3)
						{
							cerr << "missing arguments in normal, expected 3" << endl;
							break;
						}
						token = line.substr(0, pos);
						line.erase(0, pos + 1);
						nextNormal[I++] = stof(token);
					}
					nextNormal[I++] = stof(line);
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

					while ((pos = line.find(" ")) != string::npos)
					{
						if (I >= 5)
						{
							cerr << "missing arguments in face, expected 3 or 4" << endl;
							break;
						}
						token = line.substr(0, pos);
						line.erase(0, pos + 1);
						args.push_back(token);
					}
					args.push_back(line);
					// working
					size_t posi = 0;
					for (size_t i = 0; i < args.size(); i++)
					{
						vector<int> argi;
						while ((posi = args[i].find("/")) != string::npos)
						{
							if (argi.size() >= 4)
							{
								cerr << "too many arguments in faceProperties, expected 2 or 3" << endl;
								break;
							}
							token = args[i].substr(0, posi);
							args[i].erase(0, posi + 1);
							argi.push_back(stoi(token));
						}
						argi.push_back(stoi(args[i]));
						if (argi.size() < 2)
						{
							cerr << "not enough parameters in faceProperies, expected 2 or 3" << endl;
							break;
						}
					}

					if (args.size() == 3)
					{
						size_t posi = 0;
						for (size_t i = 0; i < args.size(); i++)
						{
							vector<unsigned> argi;
							while ((posi = args[i].find("/")) != string::npos)
							{
								if (argi.size() >= 4)
								{
									cerr << "too many arguments in faceProperties, expected 2 or 3" << endl;
									break;
								}
								token = args[i].substr(0, posi);
								args[i].erase(0, posi + 1);
								argi.push_back(stoi(token));
							}
							argi.push_back(stoi(args[i]));
							if (argi.size() < 2)
							{
								cerr << "not enough parameters in faceProperies 2 or 3";
								break;
							}

							vec3 placeHolder = vec3();
							if (argi.size() == 3)
								placeHolder = normals[argi[2] - 1],

								vertices.push_back(Vertex{
									coords[(argi[0]) - 1],
									vec4(1, 1, 1, 1),
									texels[(argi[1]) - 1],
									placeHolder,
									});
							indices.push_back(vertices.size() - 1);
						}
					}
					else if (args.size() == 4)
					{
						size_t posi = 0;
						for (size_t i = 0; i < args.size(); i++)
						{
							vector<unsigned> argi;
							while ((posi = line.find("/")) != string::npos)
							{
								if (argi.size() >= 4)
								{
									cerr << "too many arguments in faceProperties, expected 2 or 3" << endl;
									break;
								}
								token = line.substr(0, posi);
								line.erase(0, posi + 1);
								argi.push_back(stoi(token));
							}
							argi.push_back(stoi(line));
							if (argi.size() < 2)
							{
								cerr << "not enough parameters in faceProperies 2 or 3";
								break;
							}

							vec3 placeHolder = vec3();
							if (argi.size() == 3)
								placeHolder = normals[argi[2] - 1],
							
							vertices.push_back(Vertex{
								coords[(argi[0]) - 1],
								vec4(1, 1, 1, 1),
								texels[(argi[1]) - 1],
								placeHolder,
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
				}
			}
		}
		
		printf("loadedToBuffer %s\n", pathToFile);
		
		 for (size_t i = 0; i < coords.size(); i += 3)
		 {
		 	vec3 a = coords[indices[i]];
		 	vec3 b = coords[indices[i + 1]];
		 	vec3 c = coords[indices[i + 2]];

		 	projectVertex(a);
		 	projectVertex(b);
		 	projectVertex(c);

		 	vec2 ta = texels[indices[i]];
		 	vec2 tb = texels[indices[i + 1]];
		 	vec2 tc = texels[indices[i + 2]];

		 	rasterizeTriangle(a, b, c, ta, tb, tc);
		 }

		//std::cout << (coords.size() == indices.size() && coords.size() == texels.size() && coords.size() == normals.size() ? "same length" : "diffrent length");
		/*_coordinates = new float[coords.size() * 3];
		_indices = new unsigned[indices.size()];
		_texels = new float[texels.size() * 2];
		_normals = new float[normals.size() * 3];

		for (size_t i = 0; i < coords.size() * 3; i += 3)
		{
			float x = coords[i][0];
			float y = coords[i][1];
			float z = coords[i][2];
			_coordinates[i] = x;
			_coordinates[i + 1] = y;
			_coordinates[i + 2] = z;
		}

		for (size_t i = 0; i < normals.size(); i++)
		{
			float x = normals[i][0];
			float y = normals[i][1];
			float z = normals[i][2];
			_normals[i] = x;
			_normals[i + 1] = y;
			_normals[i + 2] = z;
		}

		for (size_t i = 0; i < texels.size(); i++)
		{
			float x = texels[i][0];
			float y = texels[i][1];
			_texels[i] = x;
			_texels[i + 1] = y;
		}

		for (size_t i = 0; i < indices.size(); i++)
		{
			_indices[i] = indices[i];
		}*/

		printf("step 1/5: transforming obj verticies to modelSpace\n");

		
		//int res = stbi_write_png("textures/res.jpg", widthImg, heightImg, 3, frameBuffer, widthImg * 3);

		//save for later

		//int centerX = widthImg / 2;
		//int centerY = heightImg / 2;
		//float angOfs = M_PI / 12;
		//float angle = M_PI / 4;

		/*append_line(centerX, centerY, 0, 0, vec3(0, 0, 0));
		append_line(centerX, centerY, widthImg, 0, vec3(255, 255, 0));
		append_line(centerX, centerY, 0, heightImg, vec3(0, 0, 255));
		append_line(centerX, centerY, widthImg, heightImg, vec3(255, 0, 0));
		append_line(600, 300, 100, 90, vec3(0, 255, 0));

		append_line(900, 900, 1920, 900, vec3(0, 0, 0));

		for (size_t i = 0; i < 9; i++)
		{
			append_line(centerX, centerY, centerX * cosf(angOfs + angle * i) * 100.f, centerY * sinf(angOfs * angle * i) * 100.f, vec3((i % 3) * 255.f, ((i + 1) % 3) * 255.f, ((i + 2) % 3) * 255.f));
		}
		saveRender(widthImg, heightImg, "textures/res10.jpg");
		
		for (size_t i = 0; i < false && vertices.size() / 3; i += 3)
		{
			vertices[indices[i]].pos = vertices[indices[i]].pos * 100.f;
			vertices[indices[i]].pos.x = vertices[indices[i]].pos.x + widthImg * .5f;
			vertices[indices[i + 1]].pos = vertices[indices[i + 1]].pos * 100.f;
			vertices[indices[i + 1]].pos.x = vertices[indices[i + 1]].pos.x + widthImg * .5f;
			vertices[indices[i + 2]].pos = vertices[indices[i + 2]].pos * 100.f;
			vertices[indices[i + 2]].pos.x = vertices[indices[i + 2]].pos.x + widthImg * .5f;

			append_line(vertices[indices[i]].pos.x, vertices[indices[i]].pos.y, vertices[indices[i + 1]].pos.x, vertices[indices[i + 1]].pos.y, vec3(255, 0, 0));
			append_line(vertices[indices[i + 1]].pos.x, vertices[indices[i + 1]].pos.y, vertices[indices[i + 2]].pos.x, vertices[indices[i + 2]].pos.y, vec3(0, 255, 0));
			append_line(vertices[indices[i + 2]].pos.x, vertices[indices[i + 2]].pos.y, vertices[indices[i]].pos.x, vertices[indices[i]].pos.y, vec3(0, 0, 255));
		}*/

		/*printf("populate faces...\n");
		Plane* faces = new Plane[coords.size() / 3];
		size_t lenFaces = sizeof(faces) / sizeof(*faces);

		printf("num of faces: %llu\n", lenFaces);
		for (size_t i = 0; i < lenFaces; i++)
		{
			vec3 a = vec3(coords[indices[i * 3]]);
			vec3 b = vec3(coords[indices[i * 3 + 1]]);
			vec3 c = vec3(coords[indices[i * 3 + 2]]);

			faces[i] = Plane(a, Cross(a - b, a - c));
		}*/

		//printf("calculating object...\n");
		

		//for (size_t y = 0; y < heightImg; y++)
		//{
		//	if (y % 100 == 0 && y != 0)
		//		printf("%f\n", y / (float)heightImg);
		//	for (size_t x = 0; x < widthImg * 3; x += 3)
		//	{
		//		for	(size_t i = 0; i < lenFaces; i += 3)
		//		{
		//			//vec2 ta = texels[indices[i]];
		//			//vec2 tb = texels[indices[i + 1]];
		//			//vec2 tc = texels[indices[i + 2]];

		//			// for every vertex check, calculate it's face plane and get the line intersection pick the depth and store which pixel is closes to the screen, no alpha support
		//			
		//			vec2 a = modelToRes(coords[indices[i]], this->widthImg, this->heightImg);
		//			vec2 b = modelToRes(coords[indices[i + 1]], this->widthImg, this->heightImg);
		//			vec2 c = modelToRes(coords[indices[i + 2]], this->widthImg, this->heightImg);
		//			vec3 coord = vec3(x, y, 0);


		//			vec3 rgb;// = this->depthPassForEachPixel(x, y, a, b, c/*, face*/);
		//			frameBuffer[x * y * widthImg] = rgb.r;
		//			frameBuffer[x * y * widthImg + 1] = rgb.g;
		//			frameBuffer[x * y * widthImg + 2] = rgb.b;
		//		}

		//	}
		//}
		////this->rasterizeTriangle(a, b, c, ta, tb, tc);
		
		//saveRender(widthImg, heightImg, "textures/res.jpg");
		// _indices = &indices[0];
		// _coordinates = &coords[0];
		// _texels = &texels[0];
		// _normals = &normals[0];
		return 1;
	}


	inline int SoftwareRenderer::saveRender(int w, int h, const char* pathToFile)
	{
		int res = stbi_write_png(pathToFile, w, h, 3, frameBuffer, w * 3);
		return res;
	}

	unsigned char* SoftwareRenderer::getTextureBuffer()
	{
		return textureBuffer;
	}

	int SoftwareRenderer::getTextureWidth()
	{
		return textureWidth;
	}

	int SoftwareRenderer::getTextureHeight()
	{
		return textureHeight;
	}

	unsigned char* SoftwareRenderer::getFrameBuffer()
	{
		return frameBuffer;
	}
	
	// CPU vertex shader
	inline void SoftwareRenderer::projectModel(const mat4& pvm)
	{

	}

	vec3 SoftwareRenderer::RasterizeLight(
		const vec3& lightSourceColor,
		const vec3& TextureRGB,
		const vec3& fragPosOut,
		const vec3& normalOut)
	{
		vec3 viewDir = Normalize(fragPosOut - cam.getPos());
		vec3 lightDir = Normalize(light.getPos() - fragPosOut);

		vec3 halfwayDir = Normalize(lightDir + viewDir);
		vec3 ambientlight = light.getIntensity() * light.getColor();

		vec3 norm = Normalize(normalOut);

		float diff = Dot(norm, lightDir) > 0.f ? Dot(norm, lightDir) : 0.f;
		vec3 diffuse = diff * light.getColor();

		float spec = powf(Dot(norm, halfwayDir) > 0.f ? Dot(norm, halfwayDir) : 0.f, 64);
		vec3 specular = light.getColor() * spec * light.getIntensity();

		vec3 out = TextureRGB * (vec3(.3f, .3f, .3f) + lightSourceColor * .7f) * vec3(ambientlight + diffuse + specular);
		if (out.x > 255.f) out.x = 255.f;
		if (out.y > 255.f) out.y = 255.f;
		if (out.z > 255.f) out.z = 255.f;
		
		if (out.x < 0.f) out.x = 0.f;
		if (out.y < 0.f) out.y = 0.f;
		if (out.z < 0.f) out.z = 0.f;

		return out;
	}

	bool SoftwareRenderer::DrawToDepthBuffer(unsigned x, unsigned y, const float depth)
	{
		// true if the dpethBuffer got updated
		//if ((x + widthImg * y) / 3 > depthBuffer.size()) return false;

		//if ((x + y) % 3 != 0)
		//{
		//	printf("x %u y %u\n", x, y);
		//	delete &x; // raise exception
		//}
		//x /= 3;
		//y /= 3;

		if (!depthBuffer[x + widthImg * y] < depth)
		{
			depthBuffer[x + widthImg * y] = depth;
			return true;
		}
		return false;
	}

	// CPU pixel shader
	// abc is window coordinates with no depth
	inline void SoftwareRenderer::rasterizeTriangle(vec3 ca, vec3 cb, vec3 cc, const vec2& ta, const vec2& tb, const vec2& tc)
	{
		// model coord
		vec2 a = vec2(ca.x, ca.y);
		vec2 b = vec2(cb.x, cb.y);
		vec2 c = vec2(cc.x, cc.y);
		
		// depth
		float da = ca.z;
		float db = cb.z;
		float dc = cc.z;

		// scale to screen
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(a);
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(b);
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(c);
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(ca);
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(cb);
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(cc);

		// scale with rgb
		a.x *= 3;
		b.x *= 3;
		c.x *= 3;
		
		ca.x *= 3;
		cb.x *= 3;
		cc.x *= 3;

		unsigned left, right, top, bottom;

		left = a.x;
		if (left > b.x) left = b.x;
		if (left > c.x) left = c.x;

		right = a.x;
		if (right < b.x) right = b.x;
		if (right < c.x) right = c.x;


		bottom = a.y;
		if (bottom > b.y) bottom = b.y;
		if (bottom > c.y) bottom = c.y;

		top = a.y;
		if (top < b.y) top = b.y;
		if (top < c.y) top = c.y;

		int trianglePass = 0;
		int depthPass = 0;

		for (unsigned y = bottom; y < top; y++)
		{
			for (unsigned x = left; x < right; x += 3)
			{
				if (isInTriangle(vec2(x, y), vec2(ca.x, ca.y), vec2(cb.x, cb.y), vec2(cc.x, cc.y)))
				{
					//trianglePass++;
					float depth = getDepthFromPixel(vec2(x, y), ca, cb, cc);
					if (!DrawToDepthBuffer(x, y, depth))
					{
						std::printf("%i %i %f %f\n", x, y, depth, depthBuffer[x + widthImg * y]);
					}
					if (DrawToDepthBuffer(x, y, depth))
					{
						//depthPass++;
						vec3 barycentric = getBarycentricCoord(vec2(x, y), vec2(ca.x, ca.y), vec2(cb.x, cb.y), vec2(cc.x, cc.y));
						if (textureBuffer != nullptr)//|| true)
						{
							vec2 textureSample = UVmapping(barycentric, ta, tb, tc);

							vec3 pixel = getPixelColorFromUV(textureBuffer, textureWidth, textureHeight, textureSample);

							/*if (pixel == vec3(-1, -1, -1))
							{
								printf("%i", 0);
								break;
							}*/
							vec2 res = vec2((ca * barycentric.x + cb * barycentric.y + cc * barycentric.z).x,
								(ca * barycentric.x + cb * barycentric.y + cc * barycentric.z).y);
							vec3 light = RasterizeLight(vec3(255, 255, 255), pixel, vec3(res, depth), vec3(0, 0, 1));

							//frameBuffer[x + y * widthImg * 3] = unsigned char(pixel.x);
							//frameBuffer[x + y * widthImg * 3 + 1] = unsigned char(pixel.y);
							//frameBuffer[x + y * widthImg * 3 + 2] = unsigned char(pixel.z);

							frameBuffer[x + y * widthImg * 3] = unsigned char(light.x);
							frameBuffer[x + y * widthImg * 3 + 1] = unsigned char(light.y);
							frameBuffer[x + y * widthImg * 3 + 2] = unsigned char(light.z);
						}
						else
						{
							//frameBuffer[x + y * widthImg * 3] = unsigned char(ca.z);
							//frameBuffer[x + y * widthImg * 3 + 1] = unsigned char(cb.z);
							//frameBuffer[x + y * widthImg * 3 + 2] = unsigned char(cc.z);
							frameBuffer[x + y * widthImg * 3] = unsigned char(barycentric.x * 255.f);
							frameBuffer[x + y * widthImg * 3 + 1] = unsigned char(barycentric.y * 255.f);
							frameBuffer[x + y * widthImg * 3 + 2] = unsigned char(barycentric.z * 255.f);
						}
					}
				}
			}
		}

		//printf("%i %i %i\n", trianglePass, depthPass, pixelCounter);
		//saveRender(widthImg, heightImg, "textures/res.jpg");
	}

	void SoftwareRenderer::triangle(vec2 a, vec2 b, vec2 c, const vec2& ta, const vec2& tb, const vec2& tc)
	{
		if (a.y == b.y && a.y == c.y) return;
		// sort the vertices, a, b, c
		if (a.y > b.y) std::swap(a, b);
		if (a.y > c.y) std::swap(a, c);
		if (b.y > c.y) std::swap(b, c);
		std::vector<unsigned char> resBuf;
		int total_height = c.y - a.y;
		for (int i = 0; i < total_height; i++)
		{
			bool second_half = i > b.y - a.y || b.y == a.y;
			int segment_height = second_half ? c.y - b.y : b.y - a.y;
			float alpha = (float)i / total_height;
			float beta  = (float)(i - (second_half ? b.y - a.y : 0)) / segment_height;
			vec2 A = a + (c - a) * alpha;
			vec2 B = second_half ? b + (c - b)*beta : a + (b - a) * beta;
			if (A.x > B.x) std::swap(A, B);
			for (int j = A.x; j <= B.x; j += 3)
			{
				vec2 point = vec2(j, a.y+i);
				vec3 barycentric = getBarycentricCoord(point, a, b, c);
				//vec2 textureSample = UVmapping(barycentric, ta, tb, tc);
				//vec3 color = getPixelColorFromUV(textureBuffer, textureWidth, textureHeight, UVmapping(barycentric, ta, tb, tc));
				
				resBuf.push_back(barycentric.x);
				resBuf.push_back(barycentric.y);
				resBuf.push_back(barycentric.z);
				//frameBuffer[(int)a.y + i + widthImg * 3 * j] = color.x;
				//frameBuffer[(int)a.y + i + widthImg * 3 * j + 1] = color.y;
				//frameBuffer[(int)a.y + i + widthImg * 3 * j + 2] = color.z;

			}
		}
		delete[] frameBuffer;
		frameBuffer = &resBuf[0];
		saveRender(widthImg, resBuf.size() * 3 / widthImg, "textures/res.jpg");
	}

	void SoftwareRenderer::setBackground(const vec3& color)
	{
		delete[] frameBuffer;
		frameBuffer = new unsigned char[widthImg * 3 * heightImg];
		for (int y = 0; y < heightImg; y++)
		{
			for (int x = 0; x < widthImg * 3; x += 3)
			{
				frameBuffer[x + widthImg * 3 * y] = color.x;
				frameBuffer[x + widthImg * 3 * y + 1] = color.y;
				frameBuffer[x + widthImg * 3 * y + 2] = color.z;
			}
		}

		for (int i = 0; i < widthImg * heightImg; i += 3)
		{
			for (int j = 0; j < 3; j++)
			frameBuffer[i + j] = unsigned char(color.data[j]);
		}
	}

	void SoftwareRenderer::moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec2& norm)
	{
		norm.x = (widthImg >> 1) * (norm.x + 1);
		norm.y = (heightImg >> 1) * (norm.y + 1);
	}

	void SoftwareRenderer::moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec3& norm)
	{
		norm.x = (widthImg >> 1) * (norm.x + 1);
		norm.y = (heightImg >> 1) * (norm.y + 1);
	}

	//vec3 barycentricToCartesian(const vec3& barycentric, const vec2& p0, const vec2& p1, const vec2& p2)
	//{
	//	return barycentric.x * p0 + barycentric.y * p1 + barycentric.z * p2;
	//}

	const float getFitSlope(const float& input_start, const float& input_end, const float& output_start, const float& output_end)
	{
		return 1.f * (output_end - output_start) / (input_end - input_start);
	}

	//float Fit(const float& input, const float& input_start, const float& output_start, const float& slope)
	//{
	//	return output_start + slope * (input - input_start);
	//}


	void Print(const mat4 m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vec4 v = m[i];
			std::cout << '(';
			for (char i = 0; i < 4; i++)
				std::cout << v.data[i] << (i == 3 ? ")\n" : ", ");
		}
	}

	bool
	ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;

		w = a = s = d = q = e = false;
		window->GetSize(width, height);

		window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
		{
			//deltatime
			switch (keycode)
			{
			case GLFW_KEY_ESCAPE: window->Close(); break;
			case GLFW_KEY_W: w = action; break;
			case GLFW_KEY_S: s = action; break;
			case GLFW_KEY_A: a = action; break;
			case GLFW_KEY_D: d = action; break;

			case GLFW_KEY_Q: q = action; break;
			case GLFW_KEY_E: e = action; break;
			}
		});

		// apply the PVM matrix (skip many steps)
		// render the zBuffer from .obj z coords
		// check if a triangles position is outside of the screen (only if error)
		// use texcoord as a color
		// use normals and dotproduct with lightcolor combined with texel color info
		
		//texture
		int widthImg = 1920, heightImg = 1200;
		softwareRenderer = new SoftwareRenderer(widthImg, heightImg);



		softwareRenderer->loadTexture("textures/evening.jpg");
		//softwareRenderer->LoadObj("textures/triangle.obj", softwareRenderer->getVertexBuffer(), softwareRenderer->getIndexBuffer());
		//softwareRenderer->LoadObj("textures/triangle.obj", loadedVertices, loadedIndices, nullptr, nullptr, nullptr, nullptr);

		//vec2 a = vec2(softwareRenderer->widthImg / 4, softwareRenderer->heightImg / 3);//rand() % softwareRenderer->widthImg, rand() % softwareRenderer->heightImg);
		//vec2 b = vec2(softwareRenderer->widthImg / 2, softwareRenderer->heightImg / 4);//rand() % softwareRenderer->widthImg, rand() % softwareRenderer->heightImg);
		//vec2 c = vec2(softwareRenderer->widthImg * 3 / 4, softwareRenderer->heightImg / 3);//rand() % softwareRenderer->widthImg, rand() % softwareRenderer->heightImg);
		//vec2 ta = vec2(0, 0);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		//vec2 tb = vec2(.5f, 1);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		//vec2 tc = vec2(1, 0);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		//
		//softwareRenderer->triangle(a, b, c, ta, tb, tc);

		// TODO: change to bresenham algorithm last, when everything else is working
		// 

		//texture
		int texW, texH, texC;
		stbi_set_flip_vertically_on_load(true);
		const unsigned char* data = stbi_load("textures/perfect.jpg", &texW, &texH, &texC, 3);
		if (!data)
		{
			fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", "textures/BETTER.jpg", stbi_failure_reason());
			exit(0);
		}
		printf("texW %i texH %i texC %i\n", texW, texH, texC);

		softwareRenderer->widthImg = widthImg;
		softwareRenderer->heightImg = heightImg;
		//std::vector<unsigned char> resBuf;
		//resBuf.resize(widthImg * heightImg * 3);
		//unsigned char* resBuf = new unsigned char[widthImg * heightImg * 3];
		//srand((unsigned)time(nullptr));

		//vec2 a = vec2(1 / 4.f, 1 / 3.f);//widthImg / 4, heightImg / 3);
		//vec2 b = vec2(1 / 2.f, 3 / 4.f);//widthImg / 2, heightImg * 3 / 4);
		//vec2 c = vec2(3 / 4.f, 1 / 3.f);//widthImg * 3 / 4, heightImg / 3);

		vec2 ta = vec2(0, 0);
		vec2 tb = vec2(.5f, 1);
		vec2 tc = vec2(1, 0);
		
		vec2 d = vec2(1, 0);
		vec2 td = vec2(.1, 0);

		// sort triangles (later

		// background
		/*softwareRenderer->setBackground(vec3(0x69, 0x69, 0x69));

		for (size_t i = 0; i < sizeof(vertices) * 3 / sizeof(float); i += 9)
		{
			vec2 a = vec2(vertices[indices[i]],
				vertices[indices[i + 1]]);
			vec2 b = vec2(vertices[indices[i + 2]],
				vertices[indices[i + 3]]);
			vec2 c = vec2(vertices[indices[i + 4]],
				vertices[indices[i + 5]]);
			softwareRenderer->rasterizeTriangle(a, b, c, ta, tb, tc);
		}*/

		if (0)
		{
			// first triangle render
			//softwareRenderer->rasterizeTriangle(a, b, c, ta, tb, tc);
			//// second triangle render
			//softwareRenderer->rasterizeTriangle(a, c, d, ta, tc, td);

			//vec2 e = vec2(0, 1);
			//softwareRenderer->moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(e);
			//vec2 te = vec2(0.5, 0.5);

			//// third triangle render
			//softwareRenderer->rasterizeTriangle(a, b, e, ta, tc, te);
		}

		stbi_flip_vertically_on_write(true);
		//int res = stbi_write_png("textures/res.jpg", widthImg, heightImg, 3, &resBuf[0], widthImg * 3 * (int)sizeof(resBuf[0]));
		//exit(0);
		//softwareRenderer->LoadObj("textures/monkey.obj");
		
		//softwareRenderer->rasterizeTriangle();
		/*vec2 a = vec2(-softwareRenderer->widthImg / 4, -softwareRenderer->heightImg / 4);
		vec2 b = vec2(softwareRenderer->widthImg / 2, softwareRenderer->heightImg * 5 / 4);
		vec2 c = vec2(softwareRenderer->widthImg * 5 / 4, -softwareRenderer->heightImg / 4);
		vec2 ta = vec2(0, 0);
		vec2 tb = vec2(.5f, 1);
		vec2 tc = vec2(1, 0);
		softwareRenderer->rasterizeTriangle(a, b, c, ta, tb, tc);
		*/
		//softwareRenderer->saveRender(softwareRenderer->widthImg, softwareRenderer->heightImg, "textures/res.jpg");
		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			// MeshResource
			cubeMesh = MeshResource::LoadObj("textures/actual_cube.obj");

			// shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->LoadShader(cubeScript->vs, cubeScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// TextureResource
			cubeTexture = std::make_shared<TextureResource>("textures/cubepic.png");
			cubeTexture->LoadFromFile();
			cubeTexture->BindTexture();

			// Actor
			Actor* cubeActor = new Actor();

			// GraphicNode
			cube = std::make_shared<GraphicNode>(cubeMesh, cubeTexture, cubeScript, cubeActor);

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	 */

	void
	ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		Camera cam(90, (float)width / height, 0.01f, 100.0f);
		cam.setPos(vec4(0, 2, 5));
		cam.setRot(vec4(0, 1, 0), M_PI);

		Lightning light(vec3(10, 10, 10), vec3(1, 1, 1), .01f);
		
		float camSpeed = .08f;

		loadedVertices = softwareRenderer->getVertexBuffer();
		loadedIndices = softwareRenderer->getIndexBuffer();


		// set identies
		cubeWorldSpaceTransform = cubeProjectionViewTransform = Translate(vec4());

		softwareRenderer->LoadObj("textures/triangle.obj", loadedVertices, loadedIndices, nullptr, nullptr, nullptr, nullptr);
		while (this->window->IsOpen())
		{
			frameIndex++;
			softwareRenderer->cam.setPos(cam.getPos() + Normalize(vec4((d - a), (q - e), (w - s))) * -camSpeed);

			for (size_t i = 0; i < loadedVertices.size(); i += 3)
			{
				vec3 a = loadedVertices[loadedIndices[i]].pos;
				vec3 b = loadedVertices[loadedIndices[i + 1]].pos;
				vec3 c = loadedVertices[loadedIndices[i + 2]].pos;
				
				softwareRenderer->projectVertex(a);
				softwareRenderer->projectVertex(b);
				softwareRenderer->projectVertex(c);


				vec2 ta = loadedVertices[loadedIndices[i]].texel;
				vec2 tb = loadedVertices[loadedIndices[i + 1]].texel;
				vec2 tc = loadedVertices[loadedIndices[i + 2]].texel;

				softwareRenderer->rasterizeTriangle(a, b, c, ta, tb, tc);
			}

			//softwareRenderer->rasterizeTriangle();

			// cube world space

			// cube view space
			cubeProjectionViewTransform = cubeWorldSpaceTransform * cam.pv();

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cubeScript->setM4(cam.pv(), "m4ProjViewPos");
			cubeTexture->LoadFromBuffer(softwareRenderer->getFrameBuffer(), softwareRenderer->widthImg, softwareRenderer->heightImg);
			cubeTexture->BindTexture();

			light.setFragShaderUniformVariables(cubeScript, cam.getPos());
			cube->DrawScene(cubeProjectionViewTransform, cubeColor);

			
			this->window->Update();
			this->window->SwapBuffers();
		}
	}

} // namespace Example
