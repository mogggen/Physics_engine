#version 430

layout(location=0) in vec4 colorOut;

out vec4 Color;

void main()
{
	Color = colorOut;
}
