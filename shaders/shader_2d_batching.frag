#version 330

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color_bloom;

uniform sampler2D sampl;

in vec2 texCoord;
in vec4 spriteColor;
flat in int isBloom;
// 0 - sprite with texture
// 1 - sprite without texture
// 2 - text (font atlas)
flat in int type;

void main()
{
    if(type == 0)
    {
        color = texture(sampl, texCoord) * spriteColor;
    }
    else if(type == 1)
    {
        color = spriteColor;
    }
    else
    {
        color = vec4(1, 1, 1, texture(sampl, texCoord).r) * spriteColor;
    }

    if(isBloom == 1)
    {
        color_bloom = color;
    }
    else
        color_bloom = vec4(0, 0, 0, 0);
}
