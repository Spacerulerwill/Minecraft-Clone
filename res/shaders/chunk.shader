#shader vertex

#version 450 core

layout (location = 0) in uvec2 data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 FragNormal;
out uint FragNormalIndex;
out vec3 FragPos;
out vec3 TexCoords;
out vec4 VertexPos;
out float isGrass;
out float isFoliage;

uniform vec3 NORMALS[6] = {
  vec3( 0, 1, 0 ),
  vec3(0, -1, 0 ),
  vec3( 1, 0, 0 ),
  vec3( -1, 0, 0 ),
  vec3( 0, 0, 1 ),
  vec3( 0, 0, -1 )
};

const float s = 64.0;
const float t = 5.0;

void main()
{
	float x = float(data.x&uint(63));
	float y = float((data.x >> 6)&uint(63));
	float z = float((data.x >> 12)&uint(63));
    
    FragNormalIndex = uint((data.x >> 18)&uint(7));
	FragNormal = NORMALS[FragNormalIndex];

	TexCoords = vec3(
		float(data.y&uint(63)),
		float((data.y >> 6)&uint(63)),
		float((data.y >> 12)&uint(255))
	);

	VertexPos = projection * view * model * vec4(x,y + (min(time - 2, 0)) * 25,z,1.0);
	FragPos = vec3(model * vec4(x,y,z, 1.0));

    isFoliage = float((data.y >> 20)&uint(1));
    isGrass = float((data.y >> 21)&uint(1));


    gl_Position = VertexPos;
   
}

#shader fragment

#version 450 core

uniform sampler2DArray tex_array;
uniform sampler2D grass_mask;

uniform vec3 grassColor = vec3(0.0, 1.0, 0.0);
uniform vec3 foliageColor = vec3(0.0, 1.0,0.0);
uniform float time;

in vec3 FragNormal;
flat in uint FragNormalIndex;
in vec3 FragPos;
in vec3 TexCoords;
in vec4 VertexPos;
in float isGrass;
in float isFoliage;
out vec4 FragColor;

void main() {
	vec4 texColor = texture(tex_array, TexCoords);

    if (texColor.a < 0.5)
		discard;
        
    if (isGrass > 0.5) {
        if (FragNormalIndex > 1) {
            vec4 grass_mask_color = texture(grass_mask, TexCoords.xy);
            grass_mask_color.rgb *= grassColor;
            texColor = mix(texColor, grass_mask_color, grass_mask_color.a);
        } else {
            texColor.rgb *= grassColor;
        }
    }
    else if (isFoliage > 0.5)
        texColor.rgb *= foliageColor;

    texColor.a = time / 3.0;
	FragColor = texColor;
}