#version 440 core

// *** In and Outputs ***
layout(points) in;
layout(triangle_strip, max_vertices=12) out;
layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_color;

// *** Textures ***
// TODO: Bind the voxel texture as input (tex_voxel).
// Make sure to sample it as an unsigned integer texture.
layout(binding = 0) uniform sampler3D tex_voxel;

// *** Buffers and Uniforms ***
layout(binding = 0, std140) uniform ubo_transform
{
	mat4 u_viewProjection;
	vec3 u_cameraPosition;
	float u_discardThresh;
};

#define POSITIVE_THRESHOLD 0.0001
#define NEGATIVE_THRESHOLD -POSITIVE_THRESHOLD

int getOppositeSign(float x)
{
	if(x > POSITIVE_THRESHOLD)
		return -1;		
	else if(x < NEGATIVE_THRESHOLD)
		return 1;
	else
		return 0;
}

// *** Entry point ***
void main()
{
	// Sample the voxel and its surrounding and decide if it must be drawn.
	ivec3 texSize = textureSize(tex_voxel, 0);
	
	ivec3 texCoord;
	texCoord.z = gl_PrimitiveIDIn / (texSize.x * texSize.y);
	texCoord.y = (gl_PrimitiveIDIn % (texSize.x * texSize.y)) / texSize.x;
	texCoord.x = (gl_PrimitiveIDIn % (texSize.x * texSize.y)) % texSize.x;
	
	vec4 texel = texelFetch(tex_voxel, texCoord, 0);
	out_color = texel;
	
	if( dot(texel.rgb, vec3(0.299, 0.587, 0.114)) < u_discardThresh )
		return;
	
	// Compute view direction to decide which faces are visible
	vec3 voxelSize = vec3(1.0);
	vec3 voxelSizeHalf = 0.5 * voxelSize;
	vec3 center = vec3(texCoord);
	vec3 viewVec = center - u_cameraPosition;
	
		
	vec3 dir[3] = {vec3(1.0,0.0,0.0), vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0)};
	for(int i = 0; i < 3; ++i)
	{
		int s = getOppositeSign(dot(viewVec,dir[i]));
		vec3 a1 = dir[i].x == 0.0? vec3(1.0,0.0,0.0) : vec3(0.0,1.0,0.0);
		vec3 a2 = vec3(1.0) - dir[i] - a1;
		
		if (s != 0) {	
			out_normal = dir[i] * s + 0.1 * a1 - 0.1 * a2;
			out_position = center + (voxelSizeHalf * dir[i] * s) + (voxelSizeHalf * a1) - (voxelSizeHalf * a2);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();

			out_normal = dir[i] * s - 0.1 * a1 - 0.1 * a2;
			out_position = center + (voxelSizeHalf * dir[i] * s) - (voxelSizeHalf * a1) - (voxelSizeHalf * a2);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = dir[i] * s + 0.1 * a1 + 0.1 * a2;
			out_position = center + (voxelSizeHalf * dir[i] * s) + (voxelSizeHalf * a1) + (voxelSizeHalf * a2);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = dir[i] * s - 0.1 * a1 + 0.1 * a2;
			out_position = center + (voxelSizeHalf * dir[i] * s) - (voxelSizeHalf * a1) + (voxelSizeHalf * a2);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			EndPrimitive();		
		}
	}
}