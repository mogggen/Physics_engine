#pragma once
#include "core/MathLibrary.h"

struct Vertex
{
	V3 pos;
	V4 rgba;
	V2 texel;
	V3 normal;

	Vertex();
	Vertex(V3 pos, V4 rgba, V2 texel, V3 normal);
};

inline
Vertex::Vertex()
{
	
}

inline
Vertex::Vertex(V3 pos, V4 rgba, V2 texel, V3 normal) :
pos(pos), rgba(rgba), texel(texel), normal(normal)
{

}