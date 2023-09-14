#shader vertex

#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment

#version 450 core

out vec4 FragColor;
in vec3 TexCoords;

uniform sampler2DArray tex_array;

void main() {
	vec4 texColor = texture(tex_array, TexCoords);
	if (texColor.a < 0.001)
		discard;
	FragColor = texColor;
}