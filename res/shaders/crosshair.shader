#shader vertex

#version 330 core
layout(location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}

#shader fragment

#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D crosshair;
uniform sampler2D screenTexture;

void main()
{
    vec4 col = texelFetch(screenTexture, ivec2(gl_FragCoord.xy), 0);
    FragColor = texture(crosshair, TexCoords) * vec4(1.0 - col.r, 1.0 - col.g, 1.0 - col.b, col.a);
    if (FragColor.a < 0.1)
        discard;
}