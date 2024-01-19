#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;

out vec3 ioVertexNormal;
out vec3 ioVertexPosition;
out vec2 vVertexTexCoord;

out vec3 viewDirTS;
out vec3 lightDirTS;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{

	ioVertexNormal = (modelMatrix * vec4(vertexNormal, 0)).xyz;
	ioVertexPosition = (modelMatrix * vec4(vertexPosition, 1)).xyz;
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	vVertexTexCoord = vertexTexCoord;


	//==================================================================//
	//==========================cwiczenia 7=============================//
	//==================================================================//


	vec3 normal = (modelMatrix * vec4(vertexNormal, 0)).xyz;
	vec3 tangent = (modelMatrix * vec4(vertexTangent, 0)).xyz;
	vec3 bitangent = (modelMatrix * vec4(vertexBitangent, 0)).xyz;

	mat3 TBN = transpose(mat3(tangent, bitangent, normal));

	vec3 viewDir  = normalize(cameraPos-ioVertexPosition);
	vec3 lightDir  = normalize(ioVertexPosition-lightPos);
	
	viewDirTS = TBN * viewDir;
	lightDirTS = TBN * lightDir;		
}
