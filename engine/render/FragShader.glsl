#version 430
layout(location=0) in vec4 Colors;
layout(location=1) in vec2 texturesOut;
layout(location=2) in vec3 fragPos;
layout(location=3) in vec3 normalOut;

uniform sampler2D textureArray;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float intensity;
uniform vec3 viewPos;
uniform float specIntensity;

out vec4 Color;

void main()
{	vec3 viewDir = normalize(fragPos - viewPos);
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 ambient = intensity * lightColor;

	vec3 norm = normalize(normalOut);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float spec = pow(max(dot(norm, halfwayDir), 0.0), 64);
	vec3 specular = lightColor * spec;  

	Color =  texture(textureArray, texturesOut)*(vec4(0.3) + Colors*0.7) * vec4(ambient + diffuse + specular, 1.0);
};
