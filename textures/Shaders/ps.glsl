#version 430

layout(location=2) in vec3 fragPos;
layout(location=0) in vec4 Colors;
layout(location=1) in vec2 texturesOut;
layout(location=3) in vec3 normalOut;

uniform sampler2D textureArray;

out vec4 Color;

void main()
{
	Color = texture(textureArray, texturesOut) * Colors;
}