#version 430
layout(location=0) in vec3 posIn;
layout(location=1) in vec4 colorIn;
layout(location=2) in vec2 texturesIn;
layout(location=3) in vec3 normalIn;

uniform sampler2D textureArray;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float intensity;
uniform vec3 viewPos;
uniform float spectralIntensity;

out vec4 colorOut;

void main()
{
	vec3 viewDirection = normalize(posIn - viewPos);
	vec3 lightningDirection = normalize(lightPos - posIn);
	vec3 halfwayDirection = normalize(lightningDirection + viewDirection);
	vec3 ambient = intensity * lightColor;

	vec3 normal = normalize(normalIn);

	float diff = max(dot(normal, lightningDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	float spec = pow(max(dot(normal, halfwayDirection), 0.0), 64);
	vec3 specular = lightColor * spec;

	colorOut = texture(textureArray, texturesIn) * (vec4(0.3) + colorIn * 0.7) * vec4(ambient + diffuse + specular, 1.0);
}