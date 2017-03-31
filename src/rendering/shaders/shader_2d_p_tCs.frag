R"(
#version 330

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color_bloom;

in vec2 texCoord;
in vec4 spriteColor;
flat in int out_isBloom;

uniform sampler2D sampl;


void main()
{
    color = texture(sampl, texCoord) * spriteColor;

    if(out_isBloom == 1)
    {
        color_bloom = color;
    }
    else
        color_bloom = vec4(0, 0, 0, color.a);
}
)"
