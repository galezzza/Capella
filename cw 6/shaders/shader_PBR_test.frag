#version 430 core

in vec3 vecNormal;
in vec3 worldPos;
in vec2 vVertexTexCoord;

in vec3 viewDirTS;
in vec3 lightDirTS;
in vec3 spotlightDirTS;

//uniform vec3 spotPos;
//uniform vec3 spotDir;
//uniform vec3 angleAf;

uniform float exp_param;
uniform float spotLightOn;

uniform sampler2D textureAlbedo;
uniform sampler2D textureNormal;
uniform sampler2D textureMetallic;
uniform sampler2D textureRoughness;
uniform sampler2D textureAO;


//TEST glow on earth hover
uniform vec3 glowColor;
uniform vec3 cameraPos;


out vec4 out_color;


const float PI = 3.14159265359;


vec3 f_lambert(vec3 c){
	return c/PI;
}

float D(vec3 n, vec3 h, float alpha){
	float numerator = pow(alpha, 2);
	float denominator;


	float firstMultiplier = pow(dot(n, h), 2);
	float secondMultiplier = pow(alpha, 2) - 1;

	denominator = PI * pow(firstMultiplier*secondMultiplier + 1, 2);

	return numerator/denominator;
}


float G(vec3 n, vec3 w_o, float k){
	float numerator = dot(n, w_o);
	float denominator = dot(n, w_o) * (1 - k) + k;

	return numerator/denominator;
}


vec3 F(vec3 h, vec3 v, vec3 F0){
	//return F0 + (1 - F0)(1 - pow(dot(h, v), 5)); bl?d!
	return F0 + (1.0 - F0) * pow(clamp(1.0 - max(dot(h, v), 0.0), 0.0, 1.0), 5.0);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


vec3 f_cook_torrance(vec3 w_i, vec3 w_o, vec3 n, vec3 h, float alpha, vec3 F0){
	float k = pow(alpha + 1, 2)/8;

	float D = D(n, h, alpha);
	float G = G(n, w_o, k);
	vec3 F = F(h, w_o, F0);

	D = DistributionGGX(n, h, alpha);        
    G = GeometrySmith(n, w_o, w_i, alpha);      

	vec3 numerator = D * F * G;
	float denominator = 4 * max(0,dot(w_o, n)) * max(0,dot(w_i, n))+0.000001;

	return numerator/denominator;
}


vec3 BRDF(vec3 w_i, vec3 w_o, vec3 c, vec3 n, vec3 h, float alpha, vec3 F0){
	vec3 k_d = 1 - F(h, w_o, F0);
	vec3 k_s = 1 - k_d;
    k_d *= 1.0 - F0;	  

	vec3 f_lambert = f_lambert(c);
	vec3 f_cook_torrance = f_cook_torrance(w_i, w_o, n, h, alpha, F0);
	
	vec3 result = (k_d * f_lambert + k_s * f_cook_torrance) * max(0,dot(w_i,n));
	//vec3 result = k_s * f_cook_torrance;
	return result;
}

vec3 PBR(vec3 p, vec3 w_i, vec3 w_o, vec3 c, vec3 n, float alpha, vec3 F0){
	//p - point - worldPos
	//w_i - lightDir - L
	//w_o - viewDir - V
	//c - color
	//n - normal
	//h - half vector
	//alpha - parametr chropowato?ci
	//F0 - parametr metaliczno?ci

	vec3 h = (w_o + w_i) / length(w_o + w_i);
	
	return BRDF(w_i, w_o, c, n, h, alpha, F0);
}

vec3 tone_mapping(vec3 illumination, float exp_param)
{
	return 1 - exp(-illumination * exp_param/100);
}

void main()
{
	//vec3 color = texture2D(colorTexture, vertexTexCoord).rgb;
	vec3 albedoTex = texture2D(textureAlbedo, vVertexTexCoord).rgb;
	vec3 normalTex= texture2D(textureNormal, vVertexTexCoord).rgb; // = vec3(0,0,1)
	vec3 metallicTex = texture2D(textureMetallic, vVertexTexCoord).rgb;
	float roughbessTex = texture2D(textureRoughness, vVertexTexCoord).r;
	vec3 aoTex = texture2D(textureAO, vVertexTexCoord).rgb;

	vec3 bySunColor = PBR(worldPos, normalize(lightDirTS), normalize(viewDirTS), albedoTex, vec3(0,0,1), roughbessTex, mix(vec3(0.4),albedoTex,metallicTex) );
	vec3 bySpotColor = PBR(worldPos, normalize(spotlightDirTS), normalize(viewDirTS), albedoTex, vec3(0,0,1), roughbessTex, mix(vec3(0.4),albedoTex,metallicTex) );

	vec3 toneSunColor = tone_mapping(bySunColor, exp_param);
	vec3 toneSpotColor = tone_mapping(bySpotColor, 400 * spotLightOn);

	out_color = vec4(toneSunColor + toneSpotColor, 1);
	
	//TEST glow on hover
	if (glowColor != vec3(-1)) {
		vec3 normalizedVertexNormal = normalize(vecNormal);
		vec3 viewDir = normalize(cameraPos - worldPos);
		float angle = dot(viewDir, normalizedVertexNormal);
		out_color = vec4(mix(glowColor, out_color.rgb, angle), 1.0);
	}
}
