#version 330

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color_bloom;

in vec2 texCoord;

uniform sampler2D sampl;

uniform vec4 spriteColor;
uniform bool isBloom;
// 0 - sprite with texture
// 1 - sprite without texture
// 2 - text (font atlas)
uniform int type;

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

    if(isBloom)
    {
        color_bloom = color;
    }
    else
        color_bloom = vec4(0, 0, 0, 0);
}
