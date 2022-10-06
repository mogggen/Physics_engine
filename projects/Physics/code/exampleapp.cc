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

	float sign(const V2& p, const V2& a, const V2& b)
	{
		return (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
	}

	bool isInTriangle(const V2& p, const V2& a, const V2& b, const V2& c)
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

	V3 getBarycentricCoord(const V2& p, const V2& a, const V2& b, const V2& c)
	{
		float L1 = ((p.y - c.y) * (b.x - c.x) + (p.x - c.x) * (c.y - b.y)) /
			((a.y - c.y) * (b.x - c.x) + (a.x - c.x) * (c.y - b.y));

		float L2 = ((p.x - L1 * a.x - c.x + L1 * c.x) / (b.x - c.x));

		float L3 = 1 - L1 - L2;

		return V3(L1, L2, L3);
	}

	// resulting barycentric coordinate p
	// texel coordinates for a-c
	V2 UVmapping(const V3& bp, const V2& ta, const V2& tb, const V2& tc)
	{
		return V2(ta * bp.x + tb * bp.y + tc * bp.z);
	}

	V3 getPixelColorFromUV(const unsigned char *texture, const int& w, const int& h, const V2& texel)
	{
		int pixelX = texel.u * w;
		int pixelY = texel.v * h;
		int index = pixelX + pixelY * w;
		if (index + 2 < w * h * 3)
			return V3(texture[index * 3], texture[index * 3 + 1], texture[index * 3 + 2]);
		return V3(-1, -1, -1);
	}

	//V3 barycentricToCartesian(const V3& barycentric, const V2& p0, const V2& p1, const V2& p2)
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

	//int SaveImage(const std::vector<char>& pixels, int w = 1024, int h = 1024)
	//{
	//	stbi_flip_vertically_on_write(1);
	//	int res = stbi_write_png("res.png", w, h, 3, &pixels[0], w * 3 * (int)sizeof(pixels[0]));
	//	return res;
	//}

	void Print(M4 m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			V4 v = m[i];
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
		stbi_set_flip_vertically_on_load(true);
		const unsigned char* data = stbi_load("textures/perfect.jpg", &texW, &texH, &texC, 3);
		if (!data)
		{
			fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", "textures/BETTER.jpg", stbi_failure_reason());
			exit(0);
		}
		printf("texW %i texH %i texC %i\n", texW, texH, texC);

		int widthImg = 1920, heightImg = 1200;
		std::vector<char> pixels;
		pixels.reserve(widthImg * heightImg * 3);
		srand((unsigned)time(nullptr));
		V2 a = V2(widthImg / 4, heightImg / 3);//rand() % widthImg, rand() % heightImg);
		V2 b = V2(widthImg / 2, heightImg * 3 / 4);//rand() % widthImg, rand() % heightImg);
		V2 c = V2(widthImg * 3 / 4, heightImg / 3);//rand() % widthImg, rand() % heightImg);
		V2 ta = V2(0, 0);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		V2 tb = V2(.5f, 1);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		V2 tc = V2(1, 0);//rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);

		for (size_t y = 0; y < heightImg; y++)
		{
			for (size_t x = 0; x < widthImg; x++)
			{
				if (isInTriangle(V2(x, y), a, b, c))
				{
					V3 barycentric = getBarycentricCoord(V2(x, y), a, b, c);
					
					if (true) // if texture is passed
					{
						V2 textureSample = UVmapping(barycentric, ta, tb, tc);

						V3 pixel = getPixelColorFromUV(data, texW, texH, textureSample);
						
						if (pixel == V3(-1, -1, -1)) break;
						pixels.push_back(unsigned char(pixel.r));
						pixels.push_back(unsigned char(pixel.g));
						pixels.push_back(unsigned char(pixel.b));
					}
					else
					{
						pixels.push_back(unsigned char(barycentric.x * 255.f));
						pixels.push_back(unsigned char(barycentric.y * 255.f));
						pixels.push_back(unsigned char(barycentric.z * 255.f));
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
		stbi_flip_vertically_on_write(true);
		int res = stbi_write_png("textures/res.png", widthImg, heightImg, 3, &pixels[0], widthImg * 3 * (int)sizeof(pixels[0]));

		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			// MeshResource
			cubeMesh = MeshResource::LoadObj("textures/quad.obj");

			cubeTexture = std::make_shared<TextureResource>("textures/res.png");

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
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LEQUAL);

		//Camera cam(90, (float)width / height, 0.01f, 100.0f);
		//cam.setPos(V4(0, 2, 5));
		//cam.setRot(V4(0, 1, 0), M_PI);

		//Lightning light(V3(10, 10, 10), V3(1, 1, 1), .01f);
		//
		//float camSpeed = .08f;

		//// set identies
		//cubeWorldSpaceTransform = cubeProjectionViewTransform = Translate(V4());

		//while (this->window->IsOpen())
		//{
		//	cam.setPos(cam.getPos() + Normalize(V4((d - a), (q - e), (w - s))) * -camSpeed);

		//	// cube world space

		//	// // cube view space
		//	cubeProjectionViewTransform = cam.pv() * cubeWorldSpaceTransform;

		//	//--------------------real-time render section--------------------
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//	cubeScript->setM4(cam.pv(), "m4ProjViewPos");

		//	light.bindLight(cubeScript, cam.getPos());
		//	cube->DrawScene(cubeProjectionViewTransform, cubeColor);

		//	
		//	this->window->Update();
		//	frameIndex++;
		//	Debug::Render(cam.pv());
		//	this->window->SwapBuffers();
		//}
	}

} // namespace Example
