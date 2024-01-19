#version 430 core

uniform vec3 color;
uniform vec3 cameraPos;

//in vec3 interpNormal;
in vec3 ioVertexNormal;
in vec3 ioVertexPosition;

out vec4 out_color;

void main()
{
	
	vec3 VVertex = cameraPos - ioVertexPosition;
	float distance = sqrt(pow(VVertex.x, 2) + pow(VVertex.y, 2) + pow(VVertex.z, 2));
	float camera_distance = sqrt(pow(cameraPos.x, 2) + pow(cameraPos.y, 2) + pow(cameraPos.z, 2));
	float camera_sun_center_distance = sqrt(pow(2, 2) + pow(camera_distance, 2));
	float max_distance = sqrt(pow(2.5, 2) + pow(camera_sun_center_distance, 2));
	float min_distance = camera_sun_center_distance - 2.5;
	float interpolate_value = (distance - min_distance) / (max_distance - min_distance);
	vec3 new_color_1 = mix(vec3(1,1,0), vec3(1,0,0), interpolate_value);
	vec3 new_color_2 = mix(vec3(1,1,0), new_color_1, interpolate_value);
	out_color = vec4(new_color_2,1);
	
	vec3 normalizeVertexNormal = normalize(ioVertexNormal);
	vec3 normalizeVVertex = normalize(VVertex);
	out_color = vec4(mix(vec3(1,0,0), vec3(1,1,0), dot(normalizeVVertex, normalizeVertexNormal)), 1);

}
