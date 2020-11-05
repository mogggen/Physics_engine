#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/Vertex.h"
#include "render/MeshResource.h"
#include "render/Camera.h"
#include "render/ShaderObject.h"
#include "render/GraphicNode.h"
#include "render/window.h"

namespace Example
{
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
		int width, height;
		float senseX, senseY;
		bool isRotate;
		bool w, a, s, d,
			q, e;
		M4 Em;
		M4 Evp;
		GLuint program;
		GLuint vertexShader;
		GLuint pixelShader;
		std::shared_ptr<MeshResource> cube;
		std::shared_ptr<ShaderObject> shaderObject;
		std::shared_ptr<GraphicNode> node;
		Display::Window* window;
	};
} // namespace Example