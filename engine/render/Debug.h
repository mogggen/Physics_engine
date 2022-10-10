#pragma once
#include "core/MathLibrary.h"

namespace Debug
{
    void Render(mat4 cameraVPMatrix);
    // void DrawSquare(float size, vec4 pos, vec4 color);
    void DrawLine(vec4 start, vec4 end, vec4 color);
}
