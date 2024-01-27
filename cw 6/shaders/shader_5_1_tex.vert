#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 transformation;
uniform mat4 modelMatrix;


out vec3 ioVertexNormal;
out vec3 ioVertexPosition;
out vec2 vVertexTexCoord;

float random(vec2 uv){
	return fract(sin(dot(uv,vec2(12.9898,78.233)))*43758.5453123);
}

void main()
{

	ioVertexNormal = (modelMatrix * vec4(vertexNormal, 0)).xyz;
	ioVertexPosition = (modelMatrix * vec4(vertexPosition, 1)).xyz;
	
	vec3 position = vertexPosition;
	position = position + vertexNormal*random(vertexTexCoord) * 0.05;

	gl_Position = transformation * vec4(position, 1.0);
	vVertexTexCoord = vertexTexCoord;
}
