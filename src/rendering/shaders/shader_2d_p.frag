R"(
#version 330

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color_bloom;

in vec2 texCoord;
in vec4 spriteColor;

uniform sampler2D sampl;

uniform bool isBloom;
// 0 - sprite with texture
// 1 - sprite without texture
uniform int type;

void main()
{
    if(type == 0)
    {
        color = texture(sampl, texCoord) * spriteColor;
    }
    else
        color = spriteColor;

    if(isBloom)
    {
        color_bloom = color;
    }
    else
        color_bloom = vec4(0, 0, 0, color.a);
}
)"
