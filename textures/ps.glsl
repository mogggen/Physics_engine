#version 430

layout(location=0) in vec4 colorOut;
layout(location=1) in vec2 texturesOut;
layout(location=2) in vec3 fragPosOut;
layout(location=3) in vec3 normalOut;

// get these 3 uniforms from lightning class
//uniform vec3 lightColor;
//uniform vec3 lightPos;
//uniform float lightIntensity;

//uniform vec3 viewPos;
uniform sampler2D textureArray;

out vec4 Color;

void main()
{
	//vec3 viewDir = normalize(fragPosOut - viewPos);
	//vec3 lightDir = normalize(lightPos - fragPosOut);

	//vec3 halfwayDir = normalize(lightDir + viewDir);
	//vec3 ambientlight = lightIntensity * lightColor;

	//vec3 norm = normalize(normalOut);

	//float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = diff * lightColor;

	//float spec = pow(max(dot(norm, halfwayDir), 0.0), 64);
	//vec3 specular = lightColor * spec;


	Color = texture(textureArray, texturesOut) * (vec4(0.3) + colorOut * 0.7); // * vec4(ambientlight + diffuse + specular, 1);
}