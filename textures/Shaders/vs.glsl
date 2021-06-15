#version 430

layout(location=0) in vec3 pos;
layout(location=1) in vec4 color;
layout(location=2) in vec2 texturesIn;
layout(location=3) in vec3 normalIn;

out vec3 fragPos;
out vec4 Colors;
out vec2 texturesOut;
out vec3 normalIn;

uniform mat4 m4;
uniform vec4 colorVector;

void main()
{
	gl_Position = m4 * vec4(pos, 1);
	Colors = colorVector * color;
	texturesOut = texturesIn;
}