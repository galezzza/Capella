#version 430 core

float AMBIENT = 0.1;

uniform vec3 cameraPos;

uniform vec3 color;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform vec3 spotlightConeDir;
uniform vec3 spotlightPhi;

uniform float exposition;

uniform sampler2D textureAlbedo;
uniform sampler2D textureNormal;
uniform sampler2D textureMetallic;
uniform sampler2D textureRoughness;
uniform sampler2D textureAO;

in vec3 vecNormal;
in vec3 worldPos;

in vec3 viewDirTS;
in vec3 lightDirTS;
in vec3 spotlightDirTS;
in vec3 test;

in vec2 vVertexTexCoord;

out vec4 outColor;


const float PI = 3.14159265359;

vec3 phongLight(vec3 lightDir, vec3 lightColor, vec3 normal,vec3 viewDir){
	float diffuse=max(0,dot(normal,lightDir));

	vec3 R = reflect(-lightDir, normal);  

	float specular = pow(max(dot(viewDir, R), 0.0), 32);

	vec3 resultColor = color*diffuse*lightColor+lightColor*specular;
	return resultColor;
}


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
	//return F0 + (1 - F0)(1 - pow(dot(h, v), 5)); bl¹d!
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
	//alpha - parametr chropowatoœci
	//F0 - parametr metalicznoœci

	vec3 h = (w_o + w_i) / length(w_o + w_i);
	
	return BRDF(w_i, w_o, c, n, h, alpha, F0);
}

void main()
{	
	//vec3 color = texture2D(colorTexture, vertexTexCoord).rgb;
	vec3 albedoTex = texture2D(textureAlbedo, vVertexTexCoord).rgb;
	vec3 normalTex= texture2D(textureNormal, vVertexTexCoord).rgb; // = vec3(0,0,1)
	vec3 metallicTex = texture2D(textureMetallic, vVertexTexCoord).rgb;
	float roughbessTex = texture2D(textureRoughness, vVertexTexCoord).r;
	vec3 aoTex = texture2D(textureAO, vVertexTexCoord).rgb;


	vec3 to_rename_color = PBR(worldPos, normalize(lightDirTS), normalize(viewDirTS), albedoTex, vec3(0,0,1), roughbessTex, mix(vec3(0.4),albedoTex,metallicTex) );
	//to_rename_color = color;


	vec3 normal = vec3(0,0,1);
	vec3 viewDir = normalize(viewDirTS);
	vec3 lightDir = normalize(lightPos-worldPos);
	vec3 ambient = AMBIENT*to_rename_color;
	vec3 attenuatedlightColor = lightColor/pow(length(lightPos-worldPos),2);
	vec3 ilumination;
	ilumination = ambient+phongLight(normalize(lightDirTS),attenuatedlightColor,normal,viewDir);
	//flashlight
	//vec3 spotlightDir= normalize(spotlightPos-worldPos);
	//float angle_atenuation = clamp((dot(-spotlightDir,spotlightConeDir)-0.8)*3,0,1);
	//
	//attenuatedlightColor = spotlightColor/pow(length(spotlightPos-worldPos),2)*angle_atenuation;
	//ilumination=ilumination+phongLight(normalize(spotlightDirTS),attenuatedlightColor,normal,viewDir);

	vec3 Li = 1.0 - exp(-ilumination*exposition);
	outColor = vec4(Li,1);

	outColor = vec4(to_rename_color, 1);
	//https://learnopengl.com/PBR/Lighting
}
