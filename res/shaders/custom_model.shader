#shader vertex

#version 450 core

layout (location = 0) in uvec2 data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;
out float isFoliage;
out vec3 FragPos;

const float ONE_SIXTEENTH = 1.0/16.0;

void main()
{
    // unpack x,y,z coordinates from 10 bit 16th representation
    uint x_packed = uint(data.x&uint(1023));
	uint y_packed = uint((data.x >> 10)&uint(1023));
	uint z_packed = uint((data.x >> 20)&uint(1023));

    float x_whole = float(x_packed / 16);
    float x_fract = (x_packed - (x_whole * 16)) * ONE_SIXTEENTH;
    float x = x_whole + x_fract;

    float y_whole = float(y_packed / 16);
    float y_fract = (y_packed - (y_whole * 16.0)) * ONE_SIXTEENTH;
    float y = y_whole + y_fract;

    float z_whole = float(z_packed / 16);
    float z_fract = (z_packed - (z_whole * 16.0)) * ONE_SIXTEENTH;
    float z = z_whole + z_fract;

    isFoliage = float((data.y >> 18)&uint(1));
    TexCoords = vec3(
        float(data.y&uint(31)) * ONE_SIXTEENTH,
		float((data.y >> 5)&uint(31)) * ONE_SIXTEENTH,
		float((data.y >> 10)&uint(255))
    );
    FragPos = vec3(model * vec4(x,y,z, 1.0));
	gl_Position = projection * view * model * vec4(x, y, z, 1.0);
}

#shader fragment

#version 450 core

in float isFoliage;
out vec4 FragColor;
in vec3 TexCoords;
in vec3 FragPos;

uniform sampler2DArray tex_array;
uniform vec3 foliageColor = vec3(0.0,1.0,0.0);
uniform ivec3 blockPos;
uniform bool drawBlockHighlight;

const float errorMargin = 0.01;

void main() {
	vec4 texColor = texture(tex_array, TexCoords);
	if (texColor.a < 0.5)
		discard;
        
    if (isFoliage > 0.5)
        texColor.rgb *= foliageColor;

    if (drawBlockHighlight && 
	FragPos.x >= blockPos.x - errorMargin && FragPos.x <= blockPos.x + 1.0 + errorMargin && 
	FragPos.y >= blockPos.y - errorMargin && FragPos.y <= blockPos.y + 1.0 + errorMargin && 
	FragPos.z >= blockPos.z - errorMargin && FragPos.z <= blockPos.z + 1.0 + errorMargin) {
		texColor.b += 0.5;
    }
	FragColor = texColor;
}