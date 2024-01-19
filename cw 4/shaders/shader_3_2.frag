#version 430 core

uniform vec3 objectColor;
in vec3 normal;

void main()
{
	gl_FragColor = vec4(normal, 1.	);
}
