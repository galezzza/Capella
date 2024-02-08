#version 430 core

uniform sampler2D colorTexture;
uniform sampler2D clouds;
uniform sampler2D normalSampler;

uniform vec3 sunPos;
uniform vec3 sunColor;
uniform float sunLightExp;

uniform vec3 lampPos;
uniform vec3 lampColor;
uniform float lampLightExp;

uniform vec3 cameraPos;
uniform float time;

vec3 normalizedVertexNormal;

in vec3 vertexPosWld;
in vec2 vertexTexCoordOut;
in vec3 viewDirTS;
in vec3 sunLightDirTS;
in vec3 lampLightDirTS;

out vec4 outColor;

vec4 calcPointLight(vec3 fragColor, vec3 lightPos, vec3 lightDirTS, vec3 lightColor, float lightExp) {
	float intensity = dot(normalizedVertexNormal, -lightDirTS);
	intensity  = max(intensity, 0.0);
	
	vec3 reflectDir = reflect(lightDirTS, normalizedVertexNormal);

	float glossPow = 8;
	float specular = pow(max(dot(viewDirTS, reflectDir), 0.0), glossPow);

	float diffuse = intensity;
	vec3 resultColor = lightColor * (fragColor * diffuse + specular );
	return vec4(1 - exp(-resultColor * lightExp), 1.0);
}


void main()
{
	vec3 textureColor = texture2D(colorTexture, vertexTexCoordOut).rgb;
	vec3 cloudColor = texture2D(clouds, vertexTexCoordOut).rgb;

	textureColor = mix(vec3(1), textureColor, cloudColor.r);
	
	//to prevent riffled clouds
	normalizedVertexNormal = vec3(0, 0, 1);
	if (cloudColor.r < 1) {
		//get normal from normal sampler
		vec3 samplerNormal = texture2D(normalSampler, vertexTexCoordOut).xyz;
		samplerNormal = 2 * samplerNormal - 1;//since sampler has values from [0, 1], but we want [-1, 1]
		normalizedVertexNormal = normalize(samplerNormal);// to avoid potential precision problems in sampler texture
	}
	
	outColor = calcPointLight(textureColor, sunPos, sunLightDirTS, sunColor, sunLightExp / length(vertexPosWld - sunPos));

	outColor += calcPointLight(textureColor, lampPos, lampLightDirTS, lampColor, lampLightExp / sqrt(max(1 , length(vertexPosWld - lampPos))));

	//Debug
	//outColor = vec4(textureColor, 1);
	
}