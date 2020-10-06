﻿#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/MathLibrary.h"
#include "core/app.h"
#include "render/window.h"
#include <memory>

namespace Example
{
	struct Vertex
	{
		V3 pos;
		V4 rgba;
		float texel[2];
	};

	class MeshResource
	{
		GLint indices;
		GLuint vertexBuffer;
		GLuint indexBuffer;
	public:
		std::shared_ptr<MeshResource> Cube(const V4 size, const V4 color);
		MeshResource(Vertex vertices[], int Verticeslength, unsigned int indices[], int indicesLength);
		~MeshResource();
		void Destroy();
		void render();
	};

	class Camera
	{
		float fov, aspect, n, f;
		V4 pos, up;
		V4 dir;
		float rad = 0.f;
	public:
		Camera(float fov, float ratio, float n, float f);
		void setPos(V4 pos);
		void setRot(V4 dir, float θ);
		M4 projectionViewMatrix();
	};

	class TextureResource
	{
		GLuint texture;
	public:
		void BindTexture();
		void LoadFromFile(const char* filename);
	};

	struct ShaderObject
	{
		GLchar* vs;
		GLchar* ps;
		GLuint program;
		void getShader(GLuint vs, GLuint ps, GLuint prog);
		void LoadShader(GLchar* vs, GLchar* ps, std::string vsPath, std::string psPath);
	};

	struct GraphicNode
	{
		std::shared_ptr<MeshResource> Geometry;
		std::shared_ptr<TextureResource> Texture;
		std::shared_ptr<ShaderObject> Shader;
		M4 Transform;
		GraphicNode(std::shared_ptr<MeshResource> geometry, std::shared_ptr<TextureResource> texture, std::shared_ptr<ShaderObject> shader, M4 transform);
	};

	class ExampleApp : public Core::App
	{
	public:
		/// constructor
		ExampleApp();
		/// destructor
		~ExampleApp();

		/// open app
		bool Open();
		/// run app
		void Run();
	private:

		GLuint program;
		GLuint vertexShader;
		GLuint pixelShader;
		std::shared_ptr<MeshResource> cube;
		std::shared_ptr<ShaderObject> shaderObject;
		std::shared_ptr<GraphicNode> node;
		Display::Window* window;
	};
} // namespace Example