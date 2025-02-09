#version 430

layout (location=0) in vec3 vertPos;
layout (location=1) in vec2 texCoord;
layout (location=2) in vec3 vertNormal;

out vec3 vertEyeSpacePos;
out vec2 tc;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout (binding=0) uniform sampler2D t;	// for texture
layout (binding=1) uniform sampler2D h;	// for height map

void main(void)
{	
    // height-mapped vertex
    float height = texture(h, texCoord).r;
    vec4 P = vec4(vertPos + (vertNormal * (height/5.0)), 1.0);

	tc = texCoord;
	
	// compute vertex position in eye space (without perspective)
	vertEyeSpacePos = (mv_matrix * P).xyz;
	gl_Position = proj_matrix * mv_matrix * P;
}
