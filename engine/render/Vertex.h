#pragma once
#include "core/MathLibrary.h"

struct Vertex
{
	vec3 pos;
	vec4 rgba;
	vec2 texel;
	vec3 normal;

	inline Vertex();
	inline Vertex(vec3 pos, vec3 rgb);
	inline Vertex(vec3 pos, vec4 rgba, vec2 texel, vec3 normal);
};

Vertex::Vertex() : pos(vec3()), rgba(vec4(vec3(), 1)), texel(vec2()), normal(vec3())
{
	
}

Vertex::Vertex(vec3 pos, vec3 rgb) : pos(pos), rgba(vec4(rgb, 1)), texel(vec2()), normal(vec3())
{

}

Vertex::Vertex(vec3 pos, vec4 rgba, vec2 texel=vec2(), vec3 normal=vec3()) : pos(pos), rgba(rgba), texel(texel), normal(normal)
{

}

