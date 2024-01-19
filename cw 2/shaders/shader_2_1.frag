#version 430 core

flat in vec4 color; 
in vec4 pos_local; 
in vec4 pos_global; 

out vec4 out_color;

uniform float time;
uniform vec4 bg_color;

void main()
{
	

	if(pos_local.x == 0.25){
		out_color.rgb=mix(vec3(1,0,0),mix(vec3(1,0,0),vec3(0,0,1),pos_local.y*15+sin(time)),pos_local.y*15+sin(time));
	}
	
	//out_color = mix(bg_color, pos_local, 1/(1 + exp(time-4)));
	//out_color = mix(bg_color, pos_global, 1/(1 + exp(time-4)));
}
