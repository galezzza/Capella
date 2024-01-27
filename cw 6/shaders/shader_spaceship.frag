#version 430 core

in vec3 ioVertexNormal;
in vec3 ioVertexPosition;
in vec2 vVertexTexCoord;

out vec4 out_color;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 lightPos;

uniform vec3 cameraPos;

uniform float exp_param;

uniform vec3 spotPos;
uniform vec3 spotDir;
uniform float angleAf;

uniform sampler2D shipTexture;
uniform sampler2D scratches;
uniform sampler2D rust;


float findz(float n, float f, float z_prim){
  float z = (-2*n*f)/(z_prim*(n-f)+n+f);
  return z;
}
vec3 get_color(vec3 newLightDir, float exp_param){
	
	vec2 vertexTexCoord = vVertexTexCoord;
	vertexTexCoord.y = 1.f - vertexTexCoord.y;
	vec3 shipColor = texture2D(shipTexture, vertexTexCoord).rgb;
	vec3 scratchesColor = texture2D(scratches, vertexTexCoord).rgb;
	vec3 rustsColor = texture2D(rust, vertexTexCoord).rgb;
	vec3 color = mix(shipColor,vec3(1,0.64,0), rustsColor.b);
	color = mix(color,vec3(1,1,1), scratchesColor.r);
	//vec3 color= rustsColor;

	float diffuse;
	float intensity;

	//vec3 newLightDir = ioVertexPosition - lightPos;

	vec3 normalizeLightDir = normalize(newLightDir);
	vec3 normalizeVertexNormal = normalize(ioVertexNormal);
	intensity = dot(normalizeVertexNormal, -normalizeLightDir);
	intensity = max(intensity, 0.0);
	vec4 out_color = vec4(color * intensity * lightColor, 1);
	diffuse = intensity;

	float specular;
	vec3 VVertex = normalize(cameraPos - ioVertexPosition);
	vec3 RVertex = reflect(normalizeLightDir, normalizeVertexNormal);
	intensity = max(dot(VVertex, RVertex), 0);
	intensity = pow(intensity, 50);
	specular  = intensity;
	float distance = pow(newLightDir.x, 2) + pow(newLightDir.y, 2) + pow(newLightDir.z, 2);

	//vec3 C_mapped = 1 - exp(-color * 1);
	vec3 illumination = (color * diffuse + specular)* lightColor/distance*100;
	//vec4 C_mapped = 1 - exp(-out_color * exp_param);

	//out_color = C_mapped;
	return illumination;
}
vec3 tone_mapping(vec3 illumination)
{
	return 1 - exp(-illumination * exp_param/100);
}
void main()
{
	vec3 color = texture2D(shipTexture, vVertexTexCoord).rgb;
	//vec3 color = vec3(0,0,1);
	//out_color = vec4(1,0,1,1);
	//out_color = vec4(color,1);
	
	float n = 0.05;
	float f = 30.;
	float z = findz(n,f, gl_FragCoord.z);
	z = z/-f;
	vec3 n_color = color * (1-z);
	n_color = mix(vec3(0,0.3f,0.3f), color, 1-z);
	out_color = vec4(n_color,1);
	out_color = vec4(color,1);
	
	vec3 newLightDir = ioVertexPosition - lightPos;
	vec3 combined_illumination = get_color(newLightDir, exp_param);
	
	vec3 spotLightDir = ioVertexPosition - spotPos;
	if (dot(normalize(spotLightDir), normalize(spotDir)) > cos(angleAf)){
		combined_illumination = combined_illumination + 0.000001 * get_color(spotLightDir, exp_param/5);
	}
	out_color = vec4(tone_mapping(combined_illumination),1);
}

