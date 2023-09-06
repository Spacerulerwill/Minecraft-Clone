#shader vertex

#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aTexCoords;
layout(location = 2) in float aAO;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;
out vec3 VertexPos;
out float AO;

void main()
{
	TexCoords = aTexCoords;
	VertexPos = aPos;
	AO = aAO;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}

#shader fragment

#version 450 core

uniform sampler2DArray tex_array;
uniform ivec3 blockPos;
uniform bool drawBlockHighlight;

in vec3 TexCoords;
in vec3 VertexPos;
in float AO;

out vec4 FragColor;

const float errorMargin = 0.0001;
const float AO_MIN = 0.3;
const float AO_PART = (1.0 - AO_MIN) / 3.0;

void main() {
	vec4 texColor = texture(tex_array, TexCoords);
	if (texColor.a < 0.1)
		discard;

	if (drawBlockHighlight && 
	VertexPos.x >= blockPos.x - errorMargin && VertexPos.x <= blockPos.x + 1.0 + errorMargin && 
	VertexPos.y >= blockPos.y - errorMargin && VertexPos.y <= blockPos.y + 1.0 + errorMargin && 
	VertexPos.z >= blockPos.z - errorMargin && VertexPos.z <= blockPos.z + 1.0 + errorMargin)
		texColor.b += 0.5;

	FragColor = texColor * (AO_MIN + (AO * AO_PART));
}