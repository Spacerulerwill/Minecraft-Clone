#shader vertex

#version 450 core

layout (location = 0) in uvec2 data;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragNormal;
out vec3 TexCoords;
out vec4 VertexPos;

uniform vec3 NORMALS[6] = {
  vec3( 0, 1, 0 ),
  vec3(0, -1, 0 ),
  vec3( 1, 0, 0 ),
  vec3( -1, 0, 0 ),
  vec3( 0, 0, 1 ),
  vec3( 0, 0, -1 )
};

void main()
{
	float x = float(data.x&uint(63));
	float y = float((data.x >> 6)&uint(63));
	float z = float((data.x >> 12)&uint(63));

	FragNormal = NORMALS[uint((data.x >> 18)&uint(7))];

	TexCoords = vec3(
		float(data.y&uint(63)),
		float((data.y >> 6)&uint(63)),
		float((data.y >> 12)&uint(255))
	);
	VertexPos = projection * view * model * vec4(x,y,z,1.0);
	gl_Position = VertexPos;
}

#shader fragment

#version 450 core

uniform sampler2DArray tex_array;
uniform vec3 waterColor = vec3(0.0, 0.0, 1.0);

in vec3 FragNormal;
in vec3 TexCoords;
in vec4 VertexPos;
out vec4 FragColor;

void main() {
	vec4 texColor = texture(tex_array, TexCoords);		
	FragColor = texColor * vec4(waterColor, 1.0);
}