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
#include "render/Lightning.h"
#include "render/ShaderResource.h"
#include "render/GraphicNode.h"
#include "render/window.h"
#include "render/Debug.h"
#include "core/Physics.h"
#include "render/Actor.h"

namespace Example
{
	struct ExampleApp
	{
		static void Run();
	};
}