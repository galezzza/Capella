#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 transformation;
uniform mat4 modelMatrix;

out vec3 ioVertexNormal;
out vec3 ioVertexPosition;
out vec2 vVertexTexCoord;
out vec3 ioLocalVertexPosition;

void main()
{
	ioLocalVertexPosition = vertexPosition;	
	ioVertexNormal = (modelMatrix * vec4(vertexNormal, 0)).xyz;
	ioVertexPosition = (modelMatrix * vec4(vertexPosition, 1)).xyz;
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	vVertexTexCoord = vertexTexCoord;
}
