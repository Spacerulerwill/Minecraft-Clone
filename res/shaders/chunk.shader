#shader vertex

#version 330 core

layout (location = 0) in uvec2 data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int LOD = 1;

out float AOMultiplier;
out vec3 TexCoords;
flat out ivec3 FragNormal;
out float isGrass;

ivec3 NORMALS[6] = ivec3[](
  vec3( 0, 1, 0 ),
  vec3(0, -1, 0 ),
  vec3( -1, 0, 0 ),
  vec3( 1, 0, 0 ),
  vec3( 0, 0, 1 ),
  vec3( 0, 0,-1)
);

const float AO_MIN = 0.3;
const float AO_PART = (1.0 - AO_MIN) / 3.0;

float calculateAOMultiplier(uint AOLevel) {
    return AO_MIN + (AOLevel * AO_PART);
}

int roundUp(int numToRound, int multiple)
{
    return ((numToRound + multiple - 1) / multiple) * multiple;
}
void main()
{
    float x = float(data.x&uint(63));
    float y = float((data.x >> 6)&uint(63));
    float z = float((data.x >> 12)&uint(63));
    uint ao = uint((data.x >> 18)&uint(3));

    uint fragNormalIndex = uint((data.y >> 20)&uint(7));
    FragNormal = NORMALS[fragNormalIndex];
    isGrass = float((data.y >> 23)&uint(1));
    
    AOMultiplier = calculateAOMultiplier(ao);
    TexCoords = vec3(
		float(data.y&uint(63)),
		float((data.y >> 6)&uint(63)),
		float((data.y >> 12)&uint(255))
	);

    gl_Position = projection * view * model * vec4(
        float(roundUp(int(x), LOD)),
        float(roundUp(int(y), LOD)),
        float(roundUp(int(z), LOD)), 
    1.0);  
}

#shader fragment

#version 330 core

in float AOMultiplier;
in vec3 TexCoords;
flat in ivec3 FragNormal;
in float isGrass;
out vec4 FragColor;

uniform sampler2DArray tex_array;
uniform sampler2D grass_mask;
uniform vec3 grassColor = vec3(145.0, 189.0, 89.0) / 255.0;
uniform float firstBufferTime;
uniform float time;

void main() {
    vec4 texColor = texture(tex_array, TexCoords);

    if (texColor.a < 0.5) {
        discard;
    }

    if (isGrass > 0.5) {
        if(FragNormal.y == 1) {
	    texColor.rgb *= grassColor;
        }
        else if (FragNormal.y != -1) {
            vec4 grass_mask_color = texture(grass_mask, TexCoords.xy);
            grass_mask_color.rgb *= grassColor;
            texColor = mix(texColor, grass_mask_color, grass_mask_color.a);
        }
    }
 
    FragColor = texColor * vec4(vec3(AOMultiplier), clamp(time - firstBufferTime, 0.0, 1.0));
}