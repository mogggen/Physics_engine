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
		// get from foo.obj
		GLuint vertexBufHandle;
		GLuint indexBufHandle;
		unsigned* vertices;
		unsigned* indices;


		//resulting frame
		unsigned char* frameBuffer;
		int rasterWidth, rasterHeight;
		
		// the texture
		unsigned char* textureBuffer;
		int textureWidth, textureHeight;
		
	public:
		SoftwareRenderer();
		void setVertexBuffer(unsigned* vertexBuffer);
		void setIndexBuffer(unsigned* indexBuffer);
		unsigned char* getFrameBuffer();
		void setFrameBuffer(unsigned char* frameBuffer);
		void projectModel(const mat4& pvm);
		const unsigned char* loadTexture(const char* pathToFile);
		int saveRender(int w, int h, const char* pathToFile);

		unsigned char* getTextureBuffer();
		int getTextureWidth();
		int getTextureHeight();

		bool isInTriangle(const vec2& p, const vec2& a, const vec2& b, const vec2& c);
		float sign(const vec2& p, const vec2& a, const vec2& b);
		vec3 getBarycentricCoord(const vec2& p, const vec2& a, const vec2& b, const vec2& c);
		vec2 UVmapping(const vec3& bp, const vec2& ta, const vec2& tb, const vec2& tc);
		vec3 getPixelColorFromUV(const unsigned char* texture, const int& w, const int& h, const vec2& texel);
		void rasterizeTriangle(const int& widthImg, const int& heightImg, int frameIndex);
	};

	SoftwareRenderer::SoftwareRenderer()
	{
		stbi_set_flip_vertically_on_load(true);
		stbi_flip_vertically_on_write(true);
	}

	void SoftwareRenderer::setFrameBuffer(unsigned char* _frameBuffer)
	{
		frameBuffer = _frameBuffer;
	}

	inline float SoftwareRenderer::sign(const vec2& p, const vec2& a, const vec2& b)
	{
		return (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
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

		return !(has_neg && has_pos);
	}

	vec3 SoftwareRenderer::getBarycentricCoord(const vec2& p, const vec2& a, const vec2& b, const vec2& c)
	{
		float L1 = ((p.y - c.y) * (b.x - c.x) + (p.x - c.x) * (c.y - b.y)) /
			((a.y - c.y) * (b.x - c.x) + (a.x - c.x) * (c.y - b.y));

		float L2 = ((p.x - L1 * a.x - c.x + L1 * c.x) / (b.x - c.x));

		float L3 = 1 - L1 - L2;

		return vec3(L1, L2, L3);
	}

	// resulting barycentric coordinate p
	// texel coordinates for a-c
	inline vec2 SoftwareRenderer::UVmapping(const vec3& bp, const vec2& ta, const vec2& tb, const vec2& tc)
	{
		return vec2(ta * bp.x + tb * bp.y + tc * bp.z);
	}

	inline vec3 SoftwareRenderer::getPixelColorFromUV(const unsigned char *texture, const int& w, const int& h, const vec2& texel)
	{
		int pixelX = texel.u * w;
		int pixelY = texel.v * h;
		int index = (pixelX + pixelY * w) * 3;
		if (index + 2 < w * h * 3)
			return vec3(texture[index], texture[index + 1], texture[index + 2]);
		return vec3(-1, -1, -1);
	}

	inline const unsigned char* SoftwareRenderer::loadTexture(const char* pathToFile)
	{
		int nrChannels;
		textureBuffer = stbi_load(pathToFile, &textureWidth, &textureHeight, &nrChannels, 3);
		if (!textureBuffer)
		{
			fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", pathToFile, stbi_failure_reason());
			exit(0);
		}
		return textureBuffer;
	}

	bool SoftwareRenderer::LoadObj(const char* pathToFile, unsigned* _indices, vec3* _coordinates, vec2* _texels, vec3* _normals)
	{
		char buf[1024];
		FILE* fs;
#ifndef __linux
		fopen_s(&fs, pathToFile, "r"); // textures/sphere.obj
#else
		fs = fopen64(pathToFile, "r"); // "textures/sphere.obj"
#endif

		unsigned long long verticesUsed = 0ull;
		std::vector<uint32_t> indices;
		std::vector<vec3> coords;
		std::vector<vec2> texels;
		std::vector<vec3> normals;
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

				else if (buf[0] == 'v' && buf[1] == 't' && buf[2] == '\0')
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

				else if (buf[0] == 'v' && buf[1] == 'n' && buf[2] == '\0')
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
		}
		else
		{
			printf("file not found with path \"./%s\"\n", pathToFile);
		}
		fclose(fs);
		printf("loadedToBuffer %s\n", pathToFile);
		_indices = &indices[0];
		_coordinates = &coords[0];
		_texels = &texels[0];
		_normals = &normals[0];
		return fs;
	}


	inline int SoftwareRenderer::saveRender(int w = 1024, int h = 1024, const char* pathToFile = "textures/res.jpg")
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
		for (size_t i = 0; i < sizeof(vertices) / sizeof(unsigned); i += 3)
		{
			vec4 temp = pvm * vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1);
			for (size_t j = 0; j < 3; j++)
			{
				vertices[i + j] = temp[j];
			}
		}
	}

	// CPU pixel shader
	inline void SoftwareRenderer::rasterizeTriangle(const int& widthImg, const int& heightImg, int frameIndex)
	{
		float step = frameIndex * 0.01f;
		frameBuffer = new unsigned char[widthImg * heightImg * 3];
		// one triangle
		vec2 a = vec2(-widthImg / 4, -heightImg / 4);
		vec2 b = vec2(widthImg / 2, heightImg * 5 / 4);
		vec2 c = vec2(widthImg * 5 / 4, -heightImg / 4);
		vec2 ta = vec2(0 + step, 0);
		vec2 tb = vec2(.5f + step, 1);
		vec2 tc = vec2(1 + step, 0);

		for (size_t y = 0; y < heightImg; y++)
		{
			for (size_t x = 0; x < widthImg; x++)
			{
				if (this->isInTriangle(vec2(x, y), a, b, c))
				{
					vec3 barycentric = this->getBarycentricCoord(vec2(x, y), a, b, c);

					if (true) // if texture is passed
					{
						vec2 textureSample = this->UVmapping(barycentric, ta, tb, tc);

						vec3 pixel = this->getPixelColorFromUV(textureBuffer, textureWidth, textureHeight, textureSample);

						if (pixel == vec3(-1, -1, -1)) break;
						frameBuffer[x + y * widthImg] = (unsigned char(barycentric.x * pixel.r));
						frameBuffer[x + y * widthImg] = (unsigned char(barycentric.y * pixel.g));
						frameBuffer[x + y * widthImg] = (unsigned char(barycentric.z * pixel.b));
					}
					else
					{
						frameBuffer[x + y * widthImg] = (unsigned char(barycentric.x * 255.f));
						frameBuffer[x + y * widthImg] = (unsigned char(barycentric.y * 255.f));
						frameBuffer[x + y * widthImg] = (unsigned char(barycentric.z * 255.f));
					}
				}
				else
				{
					// gray background
					frameBuffer[x + y * widthImg] = (0x69);
					frameBuffer[x + y * widthImg] = (0x69);
					frameBuffer[x + y * widthImg] = (0x69);
				}
			}
		}
	}

	//vec3 barycentricToCartesian(const vec3& barycentric, const vec2& p0, const vec2& p1, const vec2& p2)
	//{
	//	return barycentric.x * p0 + barycentric.y * p1 + barycentric.z * p2;
	//}

	const float getFitSlope(const float& input_start, const float& input_end, const float& output_start, const float& output_end)
	{
		return 1.0 * (output_end - output_start) / (input_end - input_start);
	}

	//float Fit(const float& input, const float& input_start, const float& output_start, const float& slope)
	//{
	//	return output_start + slope * (input - input_start);
	//}

	void Print(mat4 m)
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

		// load verticies, normals, and texture coords form .obj
		// apply the PVM matrix
		// draw the triangles faces with normals pointing towards the camera
		// if a triangle is completely covering another and has a depth value closer to the camera, don't draw the other one
		// check if a triangles position is outside of the screen
		// use texcoord as a color

		//texture
		softwareRenderer = new SoftwareRenderer;
		softwareRenderer->loadTexture("textures/evening.jpg");

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			// MeshResource
			cubeMesh = MeshResource::LoadObj("textures/cube.obj");

			// shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->LoadShader(cubeScript->vs, cubeScript->ps, "textures/vs.glsl", "textures/ps.glsl");

			// TextureResource
			cubeTexture = std::make_shared<TextureResource>("textures/res.jpg");
			//cubeTexture->LoadFromFile();
			//cubeTexture->BindTexture();

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

		// set identies
		cubeWorldSpaceTransform = cubeProjectionViewTransform = Translate(vec4());

		while (this->window->IsOpen())
		{
			cam.setPos(cam.getPos() + Normalize(vec4((d - a), (q - e), (w - s))) * -camSpeed);

			softwareRenderer->rasterizeTriangle(widthImg, heightImg, frameIndex);

			// cube world space

			// cube view space
			cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			cubeTexture->LoadFromBuffer(softwareRenderer->getTextureBuffer(), softwareRenderer->getTextureWidth(), softwareRenderer->getTextureHeight());
			cubeTexture->BindTexture();

			light.bindLight(cubeScript, cam.getPos());
			cube->DrawScene(cubeProjectionViewTransform, cubeColor);

			
			this->window->Update();
			frameIndex++;
			Debug::Render(cam.pv());
			this->window->SwapBuffers();
		}
	}

} // namespace Example
