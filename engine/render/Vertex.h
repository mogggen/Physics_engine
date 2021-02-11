#pragma once
#include "core/MathLibrary.h"

struct Vertex
{
	V3 pos;
	V4 color;
	V2 texel;
	V3 normal;

	Vertex(V3 pos, V4 color, V2 texel, V3 normal);
};

inline Vertex::Vertex(V3 pos, V4 color, V2 texel, V3 normal) : pos(pos), color(color), texel(texel), normal(normal)
{

}