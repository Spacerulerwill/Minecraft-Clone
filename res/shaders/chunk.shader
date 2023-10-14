#shader vertex

#version 450 core

layout (location = 0) in uvec2 data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragNormal;
out uint FragNormalIndex;
out vec3 FragPos;
out vec3 TexCoords;
out float AOMultiplier;
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

const float AO_MIN = 0.3;
const float AO_PART = (1.0 - AO_MIN) / 3.0;

float calculateAOMultiplier(uint AOLevel) {
    return AO_MIN + (AOLevel * AO_PART);
}

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

	FragPos = vec3(model * vec4(x,y,z, 1.0));
    isFoliage = float((data.y >> 23)&uint(1));
    isGrass = float((data.y >> 22)&uint(1));

    uint AOLevel = uint((data.y >> 20)&uint(3));
    AOMultiplier = calculateAOMultiplier(AOLevel);

    gl_Position = projection * view * model * vec4(x,y,z,1.0);  
}

#shader fragment

#version 450 core

uniform sampler2DArray tex_array;
uniform sampler2D grass_mask;

uniform vec3 grassColor = vec3(0.0, 1.0, 0.0);
uniform vec3 foliageColor = vec3(0.0, 1.0,0.0);

in vec3 FragNormal;
flat in uint FragNormalIndex;
in vec3 FragPos;
in vec3 TexCoords;
in float AOMultiplier;
in float isGrass;
in float isFoliage;
out vec4 FragColor;

const float errorMargin = 0.01;

void main() {
	vec4 texColor = texture(tex_array, TexCoords);

    if (texColor.a < 0.5)
		discard;
    
    if (isGrass > 0.5) {
        if (FragNormalIndex > 1) {
            vec4 grass_mask_color = texture(grass_mask, TexCoords.xy);
            grass_mask_color.rgb *= grassColor;
            texColor = mix(texColor, grass_mask_color, grass_mask_color.a);
        } else if (FragNormalIndex == 0){
            texColor.rgb *= grassColor;
        }
    }
    else if (isFoliage > 0.5)
        texColor.rgb *= foliageColor;

	FragColor = texColor * vec4(vec3(AOMultiplier), 1.0);
}