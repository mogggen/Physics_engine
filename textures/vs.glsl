#version 430

layout(location=0) in vec3 posIn;
layout(location=1) in vec4 colorIn;
layout(location=2) in vec2 texturesIn;
layout(location=3) in vec3 normalIn;

out vec4 colorOut;
out vec2 texturesOut;
out vec3 fragPosOut;
out vec3 normalOut;

uniform mat4 m4ProjViewPos;
uniform mat4 m4Pos;
uniform vec4 colorVector;
uniform sampler2D textureArray;

void main()
{
	gl_Position = m4ProjViewPos * m4Pos * vec4(posIn, 1);

	colorOut = colorVector * colorIn;
	texturesOut = texturesIn;
	fragPosOut = (m4Pos * vec4(posIn, 1)).xyz;
	normalOut = mat3(transpose(inverse(m4Pos))) * normalIn;
}
