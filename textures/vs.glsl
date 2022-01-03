#version 430

layout(location=0) in vec3 posIn;
layout(location=1) in vec4 colorIn;
layout(location=2) in vec2 texturesIn;
layout(location=3) in vec3 normalIn;

layout(location=0) out vec4 colorOut;
layout(location=1) out vec2 texturesOut;
layout(location=2) out vec3 fragPosOut;
layout(location=3) out vec3 normalOut;

uniform mat4 projViewMat;
uniform mat4 worldMat;
uniform vec4 colorVector;
uniform sampler2D textureArray;

void main()
{
	gl_Position = projViewMat * worldMat * vec4(posIn, 1);

	colorOut = colorVector * colorIn;
	texturesOut = texturesIn;
	fragPosOut = (worldMat * vec4(posIn, 1)).xyz;
	normalOut = mat3(inverse(worldMat)) * normalIn;
}
