#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;

uniform vec3 lightPos;
uniform vec3 spotPos;
uniform vec3 cameraPos;

out vec3 vecNormal;
out vec3 worldPos;

out vec3 viewDirTS;
out vec3 lightDirTS;
out vec3 spotlightDirTS;

out vec2 vVertexTexCoord;

float random(vec2 uv){
	return fract(sin(dot(uv,vec2(12.9898,78.233)))*43758.5453123);
}

void main()
{

	vecNormal = normalize((modelMatrix * vec4(vertexNormal, 0)).xyz);
	worldPos = (modelMatrix * vec4(vertexPosition, 1)).xyz;
	vec4 position = transformation * vec4(vertexPosition, 1.0);
	gl_Position = position + vec4(random(vertexTexCoord)* 0 * vertexNormal, 1);
	vVertexTexCoord = vertexTexCoord;


	vec3 w_tangent = normalize(mat3(modelMatrix)*vertexTangent);
	vec3 w_bitangent = normalize(mat3(modelMatrix)*vertexBitangent);
	mat3 TBN = transpose(mat3(w_tangent, w_bitangent, vecNormal));

	vec3 V = normalize(cameraPos-worldPos);
	viewDirTS = TBN*V;
	vec3 L = normalize(lightPos-worldPos);
	lightDirTS = TBN*L;
	vec3 SL = normalize(spotPos-worldPos);
	spotlightDirTS = TBN*SL;
}