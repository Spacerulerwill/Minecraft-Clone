#shader vertex

#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoords;
layout (location = 2) in float aIsFoliage;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;
out float isFoliage;

void main()
{
    isFoliage = aIsFoliage;
    TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment

#version 450 core

in float isFoliage;
out vec4 FragColor;
in vec3 TexCoords;

uniform sampler2DArray tex_array;
uniform vec3 foliageColor = vec3(0.0, 1.0,0.0);

void main() {
	vec4 texColor = texture(tex_array, TexCoords);
	if (texColor.a < 0.001)
		discard;
    if (isFoliage > 0.5 && texColor.r == texColor.b)
        texColor.rgb *= foliageColor;
	FragColor = texColor;
}