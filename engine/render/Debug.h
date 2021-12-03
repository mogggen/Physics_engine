#pragma once
#include "core/MathLibrary.h"

namespace Debug
{
    void Render(M4 cameraVPMatrix);
    // void DrawSquare(float size, V4 pos, V4 color);
    void DrawLine(V4 start, V4 end, V4 color);
}
