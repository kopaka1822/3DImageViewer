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
	
	// left or right face	
	int s = getOppositeSign(viewVec.x);		
	if (s != 0) {
		//uvec4 vs1 = texelFetch(tex_voxel, xy + ivec2(s,0), 0);
		//if((z1Mask & vs1) == uvec4(0)) {
			out_normal = vec3(s, 0.1, -0.1);
			out_position = center + vec3(s * voxelSizeHalf.x,  voxelSizeHalf.y, -voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();

			out_normal = vec3(s, -0.1, -0.1);
			out_position = center + vec3(s * voxelSizeHalf.x, -voxelSizeHalf.y, -voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = vec3(s, 0.1, 0.1);
			out_position = center + vec3(s * voxelSizeHalf.x,  voxelSizeHalf.y,  voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = vec3(s, -0.1, 0.1);
			out_position = center + vec3(s * voxelSizeHalf.x, -voxelSizeHalf.y,  voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			EndPrimitive();
		//}
	}
	
	// top or bottom face
	s = getOppositeSign(viewVec.y);	
	if (s != 0) {
		//uvec4 v1s = texelFetch(tex_voxel, xy + ivec2(0,s), 0);
		//if((z1Mask & v1s) == uvec4(0)) {
			out_normal = vec3(0.1, s, -0.1);
			out_position = center + vec3( voxelSizeHalf.x, s * voxelSizeHalf.y, -voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();

			out_normal = vec3(-0.1, s, -0.1);
			out_position = center + vec3(-voxelSizeHalf.x, s * voxelSizeHalf.y, -voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = vec3(0.1, s, 0.1);
			out_position = center + vec3( voxelSizeHalf.x, s * voxelSizeHalf.y,  voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = vec3(-0.1, s, 0.1);
			out_position = center + vec3(-voxelSizeHalf.x, s * voxelSizeHalf.y,  voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			EndPrimitive();
		//}
	}
	
	// front or back face
	s = getOppositeSign(viewVec.z);	
	if (s != 0) {
		//uvec4 zMask = computeSingleBitMask(z+s);
		//if((zMask & v11) == uvec4(0)) {
			out_normal = vec3(0.1, -0.1, s);
			out_position = center + vec3( voxelSizeHalf.x, -voxelSizeHalf.y, s * voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();

			out_normal = vec3(-0.1, -0.1, s);
			out_position = center + vec3(-voxelSizeHalf.x, -voxelSizeHalf.y, s * voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = vec3(0.1, 0.1, s);
			out_position = center + vec3( voxelSizeHalf.x,  voxelSizeHalf.y, s * voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			
			out_normal = vec3(-0.1, 0.1, s);
			out_position = center + vec3(-voxelSizeHalf.x,  voxelSizeHalf.y, s * voxelSizeHalf.z);
			gl_Position = u_viewProjection * vec4(out_position, 1);
			EmitVertex();
			EndPrimitive();
		//}
	}
}