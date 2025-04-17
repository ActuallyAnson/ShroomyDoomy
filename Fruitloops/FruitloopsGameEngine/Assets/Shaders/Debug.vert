#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat3 u_ViewProj;


out vec4 v_Color;

void main() {
    v_Color = a_Color;
    gl_Position = vec4(vec2(u_ViewProj * vec3(a_Position, 1.0)), 0.0, 1.0);
}

