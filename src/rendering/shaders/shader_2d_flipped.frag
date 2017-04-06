R"(
#version 330

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color_bloom;

in vec2 texCoord;

uniform sampler2D sampl;

uniform vec4 spriteColor;
uniform bool isBloom;

void main()
{
    color = texture(sampl, texCoord) * spriteColor;

    if(isBloom)
    {
        color_bloom = color;
    }
    else
        color_bloom = vec4(0, 0, 0, color.a);
}
)"
