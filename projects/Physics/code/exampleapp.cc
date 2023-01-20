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

	struct objBuffer
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;
	};

	struct FrameBuffer
	{
		unsigned char* colorBuffer;
		std::vector<float> depthBuffer;
		unsigned widthImg = 1920, heightImg = 1080;
	};

	class SoftwareRenderer
	{
		// the texture
		unsigned char* texture = nullptr;
		int textureWidth, textureHeight;	
	public:
		void* handle;
		objBuffer ob;
		FrameBuffer fb;
		Camera cam;
		mat4 currentRotation;
		Lightning light;
		mat4 mvp;
		SoftwareRenderer();
		SoftwareRenderer(unsigned rzw, unsigned rzh);
		void Init();

		void defaultVertexShader(Vertex& v);
		vec3 defaultFragmentShader(unsigned char* texture, vec3 position, vec2 uv, vec3 normal); // interpolate normals per face

		std::function<vec3(Vertex& v)> vertexShader;
		std::function<vec3(unsigned char* frame, vec3 pos, vec2 uv)> fragmentShader;

		void SoftwareRenderer::setModel_view_projection(const mat4 mvp);

		void setVertexShader(std::function<vec3(Vertex&)>&vertShader);
		void setFragmentShader(std::function<vec3(unsigned char*, vec3, vec2)>& fragShader);

		unsigned char* getFrameBuffer();
		void setFrameBuffer(unsigned w, unsigned h);
		void setFrameBuffer(unsigned char* frame);
		void draw(void* handle);
		bool DrawToDepthBuffer(unsigned x, unsigned y, const float depth);
		float getDepthFromPixel(const vec2& p, const vec3& a, const vec3& b, const vec3& c);
		void moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec3& norm);
		void projectModel(const mat4& pvm);
		void clearRender();
		vec3 RasterizeLight(const vec3& lightSourceColor, const vec3& TextureRGB, const vec3& fragPosOut, const vec3& normalOut);
		void loadTexture(const char* pathToFile);
		void* LoadObj(const char* pathToFile, std::vector<Vertex>& vertices, std::vector<unsigned>& indices);
		int saveRender(int w = 1920, int h = 1200, const char* pathToFile="textures/res.jpg");

		void setTextureResource(const char* pathToResource);
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
		void RasterizeTriangle(Vertex a, Vertex b, Vertex c);
	};

	SoftwareRenderer::SoftwareRenderer()
	{
		std::cerr << "CAUTION: widthImg and heightImg is set to 1920 by 1080" << std::endl;
		Init();
	}

	SoftwareRenderer::SoftwareRenderer(unsigned razWidth, unsigned razHeight)
	{
		fb.widthImg = razWidth;
		fb.heightImg = razHeight;
		Init();
	}

	void SoftwareRenderer::Init()
	{
		stbi_set_flip_vertically_on_load(true);
		stbi_flip_vertically_on_write(true);

		fb.depthBuffer.resize(fb.widthImg * fb.heightImg);

		currentRotation = (Rotation(vec4(1, 0, 0), 0) * Rotation(vec4(0, 1, 0), 0));
		fb.colorBuffer = new unsigned char[fb.widthImg * fb.heightImg * 3];

		cam = Camera(90, (float)fb.widthImg / fb.heightImg, 0.01f, 1000.0f);
		cam.setPos(vec4(0, 2, 5));
		cam.addRot(vec4(0, 1, 0), M_PI);
		for (float& f: fb.depthBuffer)
		{
			f = cam.getFarPlane();
		}

		light = Lightning(vec3(10, 10, 10), vec3(1, 1, 1), .01f);

		handle = LoadObj("textures/sphere.obj", ob.vertices, ob.indices);

		//auto lambda = &defaultVertexShader;
		//[this](auto lambda) { setVertexShader(lambda); }
		//setVertexShader(defaultVertexShader);
	}

	void SoftwareRenderer::setFrameBuffer(unsigned w, unsigned h)
	{
		if (fb.colorBuffer) delete[] fb.colorBuffer;
		fb.colorBuffer = new unsigned char[w * 3 * h];
	}

	void SoftwareRenderer::setFrameBuffer(unsigned char* _frameBuffer)
	{
		if (fb.colorBuffer) delete[] fb.colorBuffer;
		fb.colorBuffer = _frameBuffer;
	}	

	void SoftwareRenderer::setTextureResource(const char* pathToResource)
	{
		if (texture) delete[] texture;
		loadTexture(pathToResource);
	}

	void SoftwareRenderer::setVertexShader(std::function<vec3(Vertex&)>& vertShader)
	{
		this->vertexShader = vertShader;
	}

	void SoftwareRenderer::setFragmentShader(std::function<vec3(unsigned char*, vec3, vec2)>& fragShader)
	{
		this->fragmentShader = fragShader;
	}

	void SoftwareRenderer::setModel_view_projection(const mat4 mvp)
	{
		this->mvp = mvp;
	}

	void Print(mat4 m);

	inline void SoftwareRenderer::projectVertex(vec3& v)
	{
		const mat4 start = Translate(vec4(v));
		const mat4 projection = cam.pv() * (currentRotation * start);
		setModel_view_projection(projection);
		v = vec3(mvp[0][3], mvp[1][3], mvp[2][3]) / mvp[3][3];
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
		return vec3();
	}

	inline void SoftwareRenderer::loadTexture(const char* pathToFile)
	{
		int nrChannels;
		texture = stbi_load(pathToFile, &textureWidth, &textureHeight, &nrChannels, 3);
		if (!texture)
		{
			fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", pathToFile, stbi_failure_reason());
			exit(0);
		}
	}

	inline void* SoftwareRenderer::LoadObj(const char* pathToFile, std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
	{
		if (!vertices.empty()) vertices.clear();
		if (!indices.empty()) indices.clear();

		using namespace std;
		ifstream fs(pathToFile);
		string lineRemainder;

		if (!fs)
		{
			printf("file not found with path \"./%s\"\n", pathToFile);
			return nullptr;
		}

		unsigned long long verticesUsed = 0ull;
		vector<vec3> coords;
		vector<vec2> texels;
		vector<vec3> normals;

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
					vec3 nextCoordinate;

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
					vec2 nextTexel;

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
					vec3 nextNormal;

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
					if (I<3U)
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

						vertices.push_back(Vertex{
							coords[(argi[0]) - 1],
							vec4(1, 1, 1, 1),
							texels[(argi[1]) - 1],
							(argi.size() == 3 ? normals[argi[2] - 1] : vec3()),
							});
						if (args.size() == 3)
							indices.push_back(vertices.size() - 1);
					}
					if (args.size() == 4)
					{
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
		return &ob;
	}

	inline int SoftwareRenderer::saveRender(int w, int h, const char* pathToFile)
	{
		int res = stbi_write_png(pathToFile, w, h, 3, fb.colorBuffer, w * 3);
		return res;
	}

	unsigned char* SoftwareRenderer::getTextureBuffer()
	{
		return texture;
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
		return fb.colorBuffer;
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
		
		//if (out.x < 0.f) out.x = 0.f;
		//if (out.y < 0.f) out.y = 0.f;
		//if (out.z < 0.f) out.z = 0.f;

		return out;
	}

	// true if the depthBuffer got updated
	bool SoftwareRenderer::DrawToDepthBuffer(unsigned x, unsigned y, const float depth)
	{
		if (x + fb.widthImg * y >= fb.depthBuffer.size() || x + fb.widthImg * y < 0) return false;

		if (fb.depthBuffer[x + fb.widthImg * y] > depth)
		{
			fb.depthBuffer[x + fb.widthImg * y] = depth;
			return true;
		}
		return false;
	}

	const unsigned min(const unsigned lhs, const unsigned rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}

	const unsigned max(const unsigned lhs, const unsigned rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	void SoftwareRenderer::RasterizeTriangle(Vertex a, Vertex b, Vertex c) {
		if (a.pos.y == b.pos.y && a.pos.y == c.pos.y) return;

		defaultVertexShader(a); // vertexShader(a); //projectVertex()
		defaultVertexShader(b);
		defaultVertexShader(c);

		// to keep the the positions of the triangles relelative to the triangle
		vec2 ba = vec2(a.pos.x , a.pos.y);
		vec2 bb = vec2(b.pos.x, b.pos.y);
		vec2 bc = vec2(c.pos.x, c.pos.y);

		if (a.pos.y > b.pos.y) std::swap(a, b);
		if (a.pos.y > c.pos.y) std::swap(a, c);
		if (b.pos.y > c.pos.y) std::swap(b, c);
		for (int i = 0; i < c.pos.y - a.pos.y; i++)
		{
			bool lower_half = i > b.pos.y - a.pos.y || b.pos.y == a.pos.y;
			int subpart_height = lower_half ? c.pos.y - b.pos.y : b.pos.y - a.pos.y;
			float total_ratio = (float)i / (c.pos.y - a.pos.y);
			float sub_ratio = safeDivide((float)(i - (lower_half ? b.pos.y - a.pos.y : 0)), subpart_height);
			vec2 A = vec2(a.pos.x, a.pos.y) + (vec2(c.pos.x, c.pos.y) - vec2(a.pos.x, a.pos.y)) * total_ratio;
			vec2 B = lower_half ? vec2(b.pos.x, b.pos.y) + (vec2(c.pos.x, c.pos.y) - vec2(b.pos.x, b.pos.y)) * sub_ratio : vec2(a.pos.x, a.pos.y) + (vec2(b.pos.x, b.pos.y) - vec2(a.pos.x, a.pos.y)) * sub_ratio;
			if (A.x > B.x) std::swap(A, B);

			for (int j = (int)(A.x) * 3; j <= (int)(B.x) * 3; j += 3)
			{
				vec2 point = vec2((int)j, (int)a.pos.y + i);
				float depth = getDepthFromPixel(point, a.pos, b.pos, c.pos);
				vec3 normal = getBarycentricCoord(point, vec2(ba.x, ba.y), vec2(bb.x, bb.y), vec2(bc.x, bc.y));
				vec3 barycentric = getBarycentricCoord(vec2(point.x * 1 / 3.f, point.y), vec2(ba.x, ba.y), vec2(bb.x, bb.y), vec2(bc.x, bc.y));
				vec2 textureSample = UVmapping(barycentric, a.texel, b.texel, c.texel);
				if (!DrawToDepthBuffer(point.x, point.y, depth)) continue;
				vec3 color = defaultFragmentShader(texture, vec3(point, depth), textureSample, normal);

				int index = point.x + point.y * fb.widthImg * 3;

				fb.colorBuffer[index + 0] = unsigned char(color.x);
				fb.colorBuffer[index + 1] = unsigned char(color.y);
				fb.colorBuffer[index + 2] = unsigned char(color.z);
			}
		}
	}


	void SoftwareRenderer::setBackground(const vec3& color)
	{
		for (int y = 0; y < fb.heightImg; y++)
		{
			for (int x = 0; x < fb.widthImg * 3; x += 3)
			{
				fb.colorBuffer[x + fb.widthImg * 3 * y + 0] = color.x;
				fb.colorBuffer[x + fb.widthImg * 3 * y + 1] = color.y;
				fb.colorBuffer[x + fb.widthImg * 3 * y + 2] = color.z;
			}
		}

		for (size_t y = 0; y < fb.heightImg; y++)
		{
			for (size_t x = 0; x < fb.widthImg; x++)
			{
				fb.depthBuffer[x + fb.widthImg * y] = cam.getFarPlane();
			}
		}
	}

	void SoftwareRenderer::moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(vec3& norm)
	{
		norm.x = (fb.widthImg >> 1) * (norm.x + 1);
		norm.y = (fb.heightImg >> 1) * (norm.y + 1);
	}

	const int Fit(int input, int outputStart, int outputEnd, int inputStart, int inputEnd)
	{
		return (int)(outputStart + (outputEnd - outputStart) / (inputEnd - inputStart) * (input - inputStart));
	}

	const float getFitSlope(const float& input_start, const float& input_end, const float& output_start, const float& output_end)
	{
		return 1.f * (output_end - output_start) / (input_end - input_start);
	}

	inline void SoftwareRenderer::clearRender()
	{
		setBackground(vec3(0x69, 0x69, 0x69));
		for (float& f : fb.depthBuffer)
		{
			f = cam.getFarPlane();
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
				softwareRenderer->currentRotation = Rotation(vec4(1, 0, 0), senseY) * Rotation(vec4(0, 1, 0), senseX);
			}
		});

		window->SetKeyPressFunction([this](int32 keycode, int32 scancode, int32 action, int32 mods)
		{
			//deltatime
			switch (keycode)
			{
			case GLFW_KEY_ESCAPE: window->Close(); break;
			//case GLFW_KEY_BACKSPACE: softwareRenderer->setVertexShader(0); break;
			//case GLFW_KEY_ENTER: softwareRenderer->setFragmentShader(0); break;
			case GLFW_KEY_M: m = action; break;
			case GLFW_KEY_L: l = action; break;

			case GLFW_KEY_W: w = action; break;
			case GLFW_KEY_S: s = action; break;
			case GLFW_KEY_A: a = action; break;
			case GLFW_KEY_D: d = action; break;
			
			case GLFW_KEY_Q: q = action; break;
			case GLFW_KEY_E: e = action; break;
			}
		});
		
		//texture
		int widthImg = 1920, heightImg = 1200;
		softwareRenderer = new SoftwareRenderer(widthImg, heightImg);

		softwareRenderer->loadTexture("textures/evening.jpg");

		//texture
		stbi_set_flip_vertically_on_load(true);
		
		softwareRenderer->fb.widthImg = widthImg;
		softwareRenderer->fb.heightImg = heightImg;

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

	void SoftwareRenderer::defaultVertexShader(Vertex& v)
	{
		projectVertex(v.pos);

		moveNormalizedCoordsToCenterOfScreenAndScaleWithScreen(v.pos);

		v.pos.x *= 3;
	}

	vec3 SoftwareRenderer::defaultFragmentShader(unsigned char* texture, vec3 point, vec2 uv, vec3 normal)
	{
		vec3 color = getPixelColorFromUV(texture, textureWidth, textureHeight, uv);

		vec3 lit = RasterizeLight(vec3(255, 255, 255), color, point, normal);

		return lit;
	}

	void SoftwareRenderer::draw(void* handle)
	{
		objBuffer* ob = (objBuffer*)handle;
		for (size_t i = 0; i < ob->indices.size(); i += 3)
		{
			Vertex a = ob->vertices[ob->indices[i + 0]];
			Vertex b = ob->vertices[ob->indices[i + 1]];
			Vertex c = ob->vertices[ob->indices[i + 2]];

			RasterizeTriangle(a, b, c);
		}
	}

	void printVertexShader(Vertex& v)
	{
		printf("vertex position:%f %f %f\n", v.pos.x, v.pos.y, v.pos.z);
		printf("vertex uv:%f %f\n", v.texel.u, v.texel.v);
		printf("vertex normal:%f %f %f\n", v.normal.x, v.normal.y, v.normal.z);
		printf("vertex color:%f %f %f %f\n", v.rgba.r, v.rgba.g, v.rgba.b, v.rgba.a);
	}

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

		// set identies
		cubeWorldSpaceTransform = Translate(vec4());
		cubeProjectionViewTransform = Translate(vec4());

		while (this->window->IsOpen())
		{
			softwareRenderer->clearRender();
			frameIndex++;
			if (!l) // hold the L key to
				cam.setPos(cam.getPos() + Normalize(vec4((e - q), (d - a), (w - s))) * camSpeed); // move the openGL camera
			else{
				softwareRenderer->cam.setPos(softwareRenderer->cam.getPos() + Normalize(vec4((d - a), (q - e), (w - s))) * camSpeed); // move the softwareRenderer Camera
			}

			if (m)
			{
				timesPressedM++;
				switch (timesPressedM % 4)
				{
				case 0:
					softwareRenderer->handle = softwareRenderer->LoadObj("textures/triangle.obj", softwareRenderer->ob.vertices, softwareRenderer->ob.indices);
					break;
				case 1:
					softwareRenderer->handle = softwareRenderer->LoadObj("textures/actual_cube.obj", softwareRenderer->ob.vertices, softwareRenderer->ob.indices);
					break;
				case 2:
					softwareRenderer->handle = softwareRenderer->LoadObj("textures/quad.obj", softwareRenderer->ob.vertices, softwareRenderer->ob.indices);
					break;
				case 3:
					softwareRenderer->handle = softwareRenderer->LoadObj("textures/sphere.obj", softwareRenderer->ob.vertices, softwareRenderer->ob.indices);
					break;
				}
				m = false;
			}

			softwareRenderer->draw(softwareRenderer->handle);

			cubeProjectionViewTransform = cubeWorldSpaceTransform * cam.pv();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cubeScript->setM4(cam.pv(), "m4ProjViewPos");
			cubeTexture->LoadFromBuffer(softwareRenderer->getFrameBuffer(), softwareRenderer->fb.widthImg, softwareRenderer->fb.heightImg);
			cubeTexture->BindTexture();

			light.setFragShaderUniformVariables(cubeScript, cam.getPos());
			cube->DrawScene(cubeProjectionViewTransform, cubeColor);

			
			this->window->Update();
			this->window->SwapBuffers();
		}
	}

} // namespace Example
