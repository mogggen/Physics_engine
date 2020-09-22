#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/MathLibrary.h"
#include "core/app.h"
#include "render/window.h"

namespace Example
{
	struct Vertice
	{
		V3 pos;
		V4 rgba;
	};

	class MeshResource
	{
		GLint indices;
		GLuint vertexBuffer;
		GLuint indexBuffer;
	public:
		MeshResource* Cube(const V4 size, const V4 color);
		MeshResource(Vertice vertices[], int Verticeslength, unsigned int indices[], int indicesLength);
		~MeshResource();
		void Destroy();
		void render();
	};

	class Camera
	{
		float fov, aspect, n, f;
		V4 pos, up;
		V4 dir;
		float θ;
	public:
		Camera(float fov, float ratio, float n, float f);
		void setPos(V4 pos);
		void setRot(V4 dir, float θ);
		M4 projectionViewMatrix();
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
		MeshResource* cube;
		GLuint triangle;
		Display::Window* window;
	};
} // namespace Example