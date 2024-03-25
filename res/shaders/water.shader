#shader vertex

#version 330 core

layout (location = 0) in uvec2 data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    float x = float(data.x&uint(63));
    float y = float((data.x >> 6)&uint(63));
    float z = float((data.x >> 12)&uint(63));

    TexCoords = vec3(
	float(data.y&uint(63)),
	float((data.y >> 6)&uint(63)),
	float((data.y >> 12)&uint(255))
    );

    gl_Position = projection * view * model * vec4(x, z, y, 1.0);
}

#shader fragment

#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform sampler2DArray tex_array;
uniform vec3 color;
uniform float ambient;

void main() {
    vec4 texColor = texture(tex_array, TexCoords); 
    FragColor = texColor * vec4(color, 1.0) * ambient;
 }
