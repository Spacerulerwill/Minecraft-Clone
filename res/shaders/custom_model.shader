#shader vertex

#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoords;
layout (location = 2) in float aIsFoliage;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 TexCoords;
out float isFoliage;
out vec3 FragPos;

void main()
{
    isFoliage = aIsFoliage;
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos.x,aPos.y,aPos.z, 1.0));
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y + (min(time - 2, 0)) * 25, aPos.z, 1.0);
}

#shader fragment

#version 450 core

in float isFoliage;
out vec4 FragColor;
in vec3 TexCoords;
in vec3 FragPos;

uniform sampler2DArray tex_array;
uniform vec3 foliageColor = vec3(0.0,1.0,0.0);
uniform float time;
uniform ivec3 blockPos;
uniform bool drawBlockHighlight;

const float errorMargin = 0.0001;

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