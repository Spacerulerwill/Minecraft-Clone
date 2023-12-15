#shader vertex

#version 450 core

layout (location = 0) in uint data;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float AOMultiplier;

const float AO_MIN = 0.3;
const float AO_PART = (1.0 - AO_MIN) / 3.0;

float calculateAOMultiplier(uint AOLevel) {
    return AO_MIN + (AOLevel * AO_PART);
}

void main()
{
    float x = float(data&uint(63));
    float y = float((data >> 6)&uint(63));
    float z = float((data >> 12)&uint(63));
    uint ao = uint((data >> 18)&uint(3));
    
    AOMultiplier = calculateAOMultiplier(ao);
    gl_Position = projection * view * model * vec4(x,y,z, 1.0);  
}

#shader fragment

#version 450 core

in float AOMultiplier;
out vec4 FragColor;

void main() {
	FragColor = vec4(1.0) * vec4(vec3(AOMultiplier), 1.0);
}