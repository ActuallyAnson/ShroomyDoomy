#version 450 core

in vec4 v_Color;
in vec2 v_TexCoords;
in float v_TexIndex;

uniform sampler2D fontAtlas[16];
uniform vec3 textColor;

out vec4 FragColor;

void main()
{
    int index = int(v_TexIndex);
    float sampled = texture(fontAtlas[index], v_TexCoords).r; // Use the red channel for grayscale font
    FragColor = vec4(textColor * v_Color.rgb, v_Color.a * sampled); 
}
