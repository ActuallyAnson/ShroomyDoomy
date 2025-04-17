#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in float a_TexIndex; 

uniform mat3 u_ViewProj;

out vec4 v_Color;
out vec2 v_TexCoords;
out float v_TexIndex; 

void main()
{
    gl_Position = vec4(vec2(u_ViewProj * vec3(a_Position, 1.0)), 0.0, 1.0);
    v_Color = a_Color;
    v_TexCoords = a_TexCoords;
    v_TexIndex = a_TexIndex;
}
