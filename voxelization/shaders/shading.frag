#version 440 core

// *** In and Outputs ***
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_color;
layout(location = 0) out vec3 out_fragColor;

// *** Buffers and Uniforms ***
layout(binding = 0, std140) uniform ubo_transform
{
	mat4 u_viewProjection;
	vec3 u_cameraPosition;
	float u_discardThresh;
};

uniform vec3 LIGHT_DIR = vec3(0.267261242, 0.801783726, 0.534522484);

// *** Entry point ***
void main()
{
	// Quick and easy directional light
	vec3 normal = normalize(in_normal);
	float cosTheta = dot(LIGHT_DIR, normal);
	vec3 viewDir = normalize(u_cameraPosition - in_position);
	float diffuse = cosTheta * 0.5 + 0.5;
	float HdotN = dot(normalize(viewDir + LIGHT_DIR), normal);
	float specular = pow(max(0.0, HdotN), 6.0);
	
	// Sample one ray in light direction
	float visibility = 1.0;
	
	//out_fragColor = vec3((0.7 * diffuse + 0.5 * specular) * visibility) * in_color.r;
	out_fragColor = vec3(in_color.r);
}
