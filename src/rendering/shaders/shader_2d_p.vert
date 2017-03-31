R"(
#version 330

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec4 pos_size;
layout (location = 2) in vec4 color;

uniform mat4 projection;

uniform vec2 texSize;
uniform vec2 texShift;

out vec2 texCoord;
out vec4 spriteColor;

void main()
{
    gl_Position = projection * vec4(vertex.xy * pos_size.zw + pos_size.xy, 0, 1);
    texCoord = vertex.zw * texSize + texShift;
    spriteColor = color;
}
)"
