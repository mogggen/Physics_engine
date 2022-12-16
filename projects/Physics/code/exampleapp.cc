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
		senseX = prevX + (0.002 * (width / 2));
		senseY = prevY + (0.002 * (height / 2));
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
		mat4 currentRotation;
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
		void triangle(vec3 a, vec3 b, vec3 c, const vec2& ta, const vec2& tb, const vec2& tc);
		bool DrawToDepthBuffer(unsigned x, unsigned y, const float depth);
		float getDepthFromPixel(const vec2& p, const vec3& a, const vec3& b, const vec3& c);
		void moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec3& norm);
		void projectModel(const mat4& pvm);
		void clearRender();
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
		void rasterizeTriangle(vec3 a, vec3 b, vec3 c, const vec2& ta, const vec2& tb, const vec2& tc);
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

		currentRotation = (Rotation(vec4(1, 0, 0), 0) * Rotation(vec4(0, 1, 0), 0));
		frameBuffer = new unsigned char[widthImg * heightImg * 3];

		cam = Camera(90, (float)widthImg / heightImg, 0.01f, 100.0f);
		cam.setPos(vec4(0, 2, 5));
		cam.addRot(vec4(0, 1, 0), M_PI);

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

	void Print(mat4 m);

	inline void SoftwareRenderer::projectVertex(vec3& v)
	{
		const mat4 start = Translate(vec4(v));
		const mat4 projection = cam.pv() * (currentRotation * start);
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
		if (texel.u > 1.f) texel.u -= (int)texel.u;
		if (texel.u < 0.f) texel.u += (int)std::abs(texel.u) + 1;

		if (texel.v > 1.f) texel.v -= (int)texel.v;
		if (texel.v < 0.f) texel.v += (int)std::abs(texel.v) + 1;
		
		int pixelX = (texel.u) * w;
		int pixelY = (texel.v) * h;
		int index = (pixelX + pixelY * w) * 3;
		if (index + 2 < textureWidth * 3 * textureHeight)
		return vec3(texture[index], texture[index + 1], texture[index + 2]);
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
		fopen_s(&fs, pathToFile, "r");
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
		return fs;
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

		vec3 out = TextureRGB * (lightSourceColor * .7f) * vec3(ambientlight + diffuse + specular);
		
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
		if (x + widthImg * y >= depthBuffer.size()) return false;

		if (!depthBuffer[x + widthImg * y] < depth)
		{
			depthBuffer[x + widthImg * y] = depth;
			return true;
		}
		return false;
	}

	void SoftwareRenderer::triangle(vec3 a, vec3 b, vec3 c, const vec2& ta, const vec2& tb, const vec2& tc) {
		if (a.y == b.y && a.y == c.y) return;

		// scale to screen
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(a);
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(b);
		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(c);

		// scale with color channels (rgb)
		a.x *= 3;
		b.x *= 3;
		c.x *= 3;

		// to keep the the positions of the triangles relelative to the triangle
		vec2 ba = vec2(a.x , a.y);
		vec2 bb = vec2(b.x, b.y);
		vec2 bc = vec2(c.x, c.y);

		if (a.y > b.y) std::swap(a, b);
		if (a.y > c.y) std::swap(a, c);
		if (b.y > c.y) std::swap(b, c);
		int total_height = c.y - a.y;
		for (int i = 0; i < total_height; i++)
		{
			bool lower_half = i > b.y - a.y || b.y == a.y;
			int subpart_height = lower_half ? c.y - b.y : b.y - a.y;
			float total_ratio = (float)i / total_height;
			float sub_ratio = safeDivide((float)(i - (lower_half ? b.y - a.y : 0)), subpart_height);
			vec2 A = vec2(a.x, a.y) + (vec2(c.x, c.y) - vec2(a.x, a.y)) * total_ratio;
			vec2 B = lower_half ? vec2(b.x, b.y) + (vec2(c.x, c.y) - vec2(b.x, b.y)) * sub_ratio : vec2(a.x, a.y) + (vec2(b.x, b.y) - vec2(a.x, a.y)) * sub_ratio;
			if (A.x > B.x) std::swap(A, B);
			for (int j = (int)(A.x) * 3; j <= (int)(B.x) * 3; j += 3)
			{
				vec2 point = vec2((int)j, (int)a.y+i);
				float depth = getDepthFromPixel(point, a, b, c);
				if (DrawToDepthBuffer((int)j, (int)a.y + i, depth))
				{
					int index = j + ((int)a.y + i) * widthImg * 3;
					if (index < 0) continue;
					if (index >= widthImg * 3 * heightImg) break;
					vec3 barycentric = getBarycentricCoord(vec2(point.x * 1/3.f, point.y), vec2(ba.x, ba.y), vec2(bb.x, bb.y), vec2(bc.x, bc.y));
					vec2 textureSample = UVmapping(barycentric, ta, tb, tc);
					vec3 color = getPixelColorFromUV(textureBuffer, textureWidth, textureHeight, textureSample);

					vec2 res = vec2((a * barycentric.x + b * barycentric.y + c * barycentric.z).x,
						(a * barycentric.x + b * barycentric.y + c * barycentric.z).y);
					color = RasterizeLight(vec3(255, 255, 255), color, vec3(j, (int)a.y + i, depth), vec3(0, 0, 1));

					if (textureBuffer != nullptr)
					{
						frameBuffer[index] = unsigned char(color.x);
						frameBuffer[index + 1] = unsigned char(color.y);
						frameBuffer[index + 2] = unsigned char(color.z);
					}
					else
					{
						frameBuffer[index] = unsigned char(barycentric.x * 255.f);
						frameBuffer[index + 1] = unsigned char(barycentric.y * 255.f);
						frameBuffer[index + 2] = unsigned char(barycentric.z * 255.f);
					}
				}
			}
		}
	}

	void SoftwareRenderer::setBackground(const vec3& color)
	{
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

	void SoftwareRenderer::moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec3& norm)
	{
		norm.x = (widthImg >> 1) * (norm.x + 1);
		norm.y = (heightImg >> 1) * (norm.y + 1);
	}

	const float getFitSlope(const float& input_start, const float& input_end, const float& output_start, const float& output_end)
	{
		return 1.f * (output_end - output_start) / (input_end - input_start);
	}

	inline void SoftwareRenderer::clearRender()
	{
		setBackground(vec3(0x69, 0x69, 0x69));
		for (float f : depthBuffer)
		{
			f = std::numeric_limits<float>::lowest();
		}
	}


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

		window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
		{
			isRotate = button == GLFW_MOUSE_BUTTON_1 && action;
			if (!isRotate)
			{
				prevX = senseX;
				prevY = senseY;
			}
		});

		window->SetMouseMoveFunction([this](float64 x, float64 y)
		{
			if (isRotate)
			{
				senseX = prevX + (0.002 * (x - width / 2));
				senseY = prevY + (0.002 * (y - height / 2));
				softwareRenderer->currentRotation = (Rotation(vec4(1, 0, 0), senseY) * Rotation(vec4(0, 1, 0), senseX));
			}
		});

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
			case GLFW_KEY_L: l = action; break;

			case GLFW_KEY_Q: q = action; break;
			case GLFW_KEY_E: e = action; break;
			}
		});
		
		//texture
		int widthImg = 1920, heightImg = 1200;
		softwareRenderer = new SoftwareRenderer(widthImg, heightImg);

		softwareRenderer->loadTexture("textures/perfect_test.jpg");

		//texture
		stbi_set_flip_vertically_on_load(true);
		
		softwareRenderer->widthImg = widthImg;
		softwareRenderer->heightImg = heightImg;

		stbi_flip_vertically_on_write(true);
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
		cam.addRot(vec4(0, 0, 1), -M_PI / 2);
		cam.addRot(vec4(0, 1, 0), M_PI);

		Lightning light(vec3(10, 10, 10), vec3(1, 1, 1), .01f);
		
		float camSpeed = .08f;

		loadedVertices = softwareRenderer->getVertexBuffer();
		loadedIndices = softwareRenderer->getIndexBuffer();


		// set identies
		cubeWorldSpaceTransform = Translate(vec4());
		cubeProjectionViewTransform = Translate(vec4());

		softwareRenderer->LoadObj("textures/triangle.obj", loadedVertices, loadedIndices, nullptr, nullptr, nullptr, nullptr);
		while (this->window->IsOpen())
		{
			softwareRenderer->clearRender();
			frameIndex++;
			if (!l) // hold the L key to
				cam.setPos(cam.getPos() + Normalize(vec4((e - q), (d - a), (w - s))) * camSpeed); // move the openGL camera
			else
				softwareRenderer->cam.setPos(softwareRenderer->cam.getPos() + Normalize(vec4((d - a), (q - e), (w - s))) * camSpeed); // move the softwareRenderer Camera

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
				
				softwareRenderer->triangle(a, b, c, ta, tb, tc);
			}

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
