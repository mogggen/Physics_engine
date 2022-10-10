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
		SoftwareRenderer();
		GLuint vertexBufHandle;
		GLuint indexBufHandle;
		unsigned char* frameBuffer;
		int textureWidth, textureHeight;
		int width, height;
	public:
		void rasterizeTriangle();
		static const unsigned char* loadTexture(const char* pathToFile, int& texW, int& texH, int& texC);
		static int saveRender(const std::vector<char>& pixels, int w, int h, const char* pathToFile);

		static bool isInTriangle(const vec2& p, const vec2& a, const vec2& b, const vec2& c);
		static float sign(const vec2& p, const vec2& a, const vec2& b);
		static vec3 getBarycentricCoord(const vec2& p, const vec2& a, const vec2& b, const vec2& c);
		static vec2 UVmapping(const vec3& bp, const vec2& ta, const vec2& tb, const vec2& tc);
		static vec3 getPixelColorFromUV(const unsigned char* texture, const int& w, const int& h, const vec2& texel);
	};

	SoftwareRenderer::SoftwareRenderer()
	{
		stbi_set_flip_vertically_on_load(true);
		stbi_flip_vertically_on_write(true);
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

	inline const unsigned char* SoftwareRenderer::loadTexture(const char* pathToFile , int& texW, int& texH, int& texC)
	{
		const unsigned char* data = stbi_load(pathToFile, &texW, &texH, &texC, 3);
		if (!data)
		{
			fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", pathToFile, stbi_failure_reason());
			exit(0);
		}
		return data;
	}
	inline int SoftwareRenderer::saveRender(const std::vector<char>& pixels, int w = 1024, int h = 1024, const char* pathToFile = "textures/res.jpg")
	{
		int res = stbi_write_png(pathToFile, w, h, 3, &pixels[0], w * 3 * (int)sizeof(pixels[0]));
		return res;
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
		int texW, texH, texC;
		const unsigned char* data = SoftwareRenderer::loadTexture("textures/evening.jpg", texW, texH, texC);

		int widthImg = 1920, heightImg = 1200;
		std::vector<char> pixels;
		pixels.reserve(size_t(widthImg * heightImg * 3));
		srand((unsigned)time(nullptr));
		vec2 a = vec2(-widthImg / 4, -heightImg / 4);//rand() % widthImg, rand() % heightImg);
		vec2 b = vec2(widthImg / 2, heightImg * 5 / 4);//rand() % widthImg, rand() % heightImg);
		vec2 c = vec2(widthImg * 5 / 4, -heightImg / 4);//rand() % widthImg, rand() % heightImg);
		vec2 ta = vec2(0, 0);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		vec2 tb = vec2(.5f, 1);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		vec2 tc = vec2(1, 0);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);

		for (size_t y = 0; y < heightImg; y++)
		{
			for (size_t x = 0; x < widthImg; x++)
			{
				if (SoftwareRenderer::isInTriangle(vec2(x, y), a, b, c))
				{
					vec3 barycentric = SoftwareRenderer::getBarycentricCoord(vec2(x, y), a, b, c);
					
					if (true) // if texture is passed
					{
						vec2 textureSample = SoftwareRenderer::UVmapping(barycentric, ta, tb, tc);

						vec3 pixel = SoftwareRenderer::getPixelColorFromUV(data, texW, texH, textureSample);
						
						if (pixel == vec3(-1, -1, -1)) break;
						pixels.push_back(unsigned char(barycentric.x * pixel.r));
						pixels.push_back(unsigned char(barycentric.y * pixel.g));
						pixels.push_back(unsigned char(barycentric.z * pixel.b));
					}
					else
					{
						pixels.push_back(unsigned char(255.f));
						pixels.push_back(unsigned char(255.f));
						pixels.push_back(unsigned char(255.f));
					}
				}
				else
				{
					// gray background
					pixels.push_back(0x69);
					pixels.push_back(0x69);
					pixels.push_back(0x69);
				}
			}
		}
		SoftwareRenderer::saveRender(pixels, widthImg, heightImg);
		//int res = stbi_write_png("textures/res.png", widthImg, heightImg, 3, &pixels[0], widthImg * 3 * (int)sizeof(pixels[0]));

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			// MeshResource
			cubeMesh = MeshResource::LoadObj("textures/quad.obj");

			cubeTexture = std::make_shared<TextureResource>("textures/res.jpg");

			// shaderResource
			cubeScript = std::make_shared<ShaderResource>();
			cubeScript->LoadShader(cubeScript->vs, cubeScript->ps, "textures/vs.glsl", "textures/ps.glsl");
			// note: bindTexture() still requires a texture, but just won't use it

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

			// cube world space

			// // cube view space
			cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;

			//--------------------real-time render section--------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cubeScript->setM4(cam.pv(), "m4ProjViewPos");

			light.bindLight(cubeScript, cam.getPos());
			cube->DrawScene(cubeProjectionViewTransform, cubeColor);

			
			this->window->Update();
			frameIndex++;
			Debug::Render(cam.pv());
			this->window->SwapBuffers();
		}
	}

} // namespace Example
