#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMat;

uniform vec3 cameraPos;
uniform vec3 sunPos;
uniform vec3 lampPos;

out vec3 vertexPosWld;
out vec2 vertexTexCoordOut;
out vec3 viewDirTS;
out vec3 sunLightDirTS;
out vec3 lampLightDirTS;

void main()
{
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	vec3 normal = (modelMat * vec4(vertexNormal, 0.0)).xyz;
	vec3 tangent = (modelMat * vec4(vertexTangent, 0.0)).xyz;
	vec3 bitangent = (modelMat * vec4(vertexBitangent, 0.0)).xyz;
	vertexPosWld = (modelMat * vec4(vertexPosition, 1.0)).xyz;
	vertexTexCoordOut = vertexTexCoord;
	vertexTexCoordOut.y = 1 - vertexTexCoord.y;// corrects inversion (bottom at top) of the earth

	mat3 TBN = transpose(mat3(tangent, bitangent, normal));
	
	vec3 sunLightDir = normalize(vertexPosWld - sunPos);
	vec3 lampLightDir = normalize(vertexPosWld - lampPos);
	vec3 viewDir = normalize(cameraPos - vertexPosWld);
	
	// tangent space
	viewDirTS = TBN * viewDir;
	sunLightDirTS = TBN * sunLightDir;
	lampLightDirTS = TBN * lampLightDir;
	
	//TODO should normilize here or in the fragment shader?
	viewDirTS = normalize(viewDirTS);
	sunLightDirTS = normalize(sunLightDirTS);
	lampLightDirTS = normalize(lampLightDirTS);
}

