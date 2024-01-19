#version 430 core

out vec4 out_color;

uniform vec3 color;

float findz(float n, float f, float z_prim){
  float z = (-2*n*f)/(z_prim*(n-f)+n+f);
  return z;
}

void main()
{
	//out_color = vec4(1,0,1,1);
	//out_color = vec4(color,1);

	float n = 0.05;
	float f = 30.;
	float z = findz(n,f, gl_FragCoord.z);
	z = z/-f;
	vec3 n_color = color * (1-z);
	n_color = mix(vec3(0,0.3f,0.3f), color, 1-z);
	out_color = vec4(n_color,1);
	//out_color = vec4(color,1);
}

