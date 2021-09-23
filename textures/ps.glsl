#version 430

layout(location=0) in vec4 Colors;
layout(location=1) in vec2 texturesOut;

uniform sampler2D textureArray;

out vec4 Color;

void main()
{
	Color = /*texture(textureArray, texturesOut) */ Colors;
}