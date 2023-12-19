#shader vertex

#version 330 core

layout (location = 0) in uvec2 data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int LOD = 1;

out float AOMultiplier;
out vec3 TexCoords;

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
out vec4 FragColor;
uniform sampler2DArray tex_array;

void main() {
        vec4 texColor = texture(tex_array, TexCoords);
	FragColor = texColor * vec4(vec3(AOMultiplier), 1.0);
}