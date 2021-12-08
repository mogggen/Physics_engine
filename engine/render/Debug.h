#pragma once
#include "core/MathLibrary.h"
#include "render/ShaderResource.h"
#include "render/MeshResource.h"

namespace Debug
{
    void Render(M4 cameraVPMatrix);
    void DrawAABB(const MeshResource& mesh, V4 color, M4 modelMatrix);
    void DrawBB(const MeshResource& mesh, V4 color, M4 modelMatrix);
    void DrawLine(V4 start, V4 end, V4 color);
}
