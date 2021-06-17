#version 430
layout(location=0) in vec3 posIn;
layout(location=1) in vec4 colorIn;
layout(location=2) in vec2 texturesIn;
layout(location=3) in vec3 normalIn;

out vec3 posOut;
out vec4 colorOut;
out vec2 texturesOut;
out vec3 normalOut;

uniform mat4 posMat;
uniform vec4 colorVec;
uniform sampler2D textureArray;

void main()
{
	gl_Position = posMat * vec4(posIn, 1);
	colorOut = colorVec * colorIn;
	texturesOut = texturesIn;

	posOut = (posMat*vec4(posIn, 1)).xyz;
	normalOut = mat3(transpose(inverse(posMat))) * normalIn;
}