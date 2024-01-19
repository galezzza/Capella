#version 430 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexColor;

uniform mat4 transformation;

flat out vec4 color;
out vec4 pos_local;
out vec4 pos_global;

void main()
{
	pos_local = vertexPosition;
	pos_global = transformation * vertexPosition;

	color = vertexColor;
	gl_Position = transformation * vertexPosition;
}
