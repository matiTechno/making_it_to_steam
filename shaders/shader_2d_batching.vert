#version 330

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec4 texData;
layout (location = 2) in vec4 color;
layout (location = 3) in ivec2 bloom_type;
layout (location = 4) in mat4 model;

out vec2 texCoord;
out vec4 spriteColor;
flat out int isBloom;
flat out int type;

uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0, 1);
    texCoord = vertex.zw * texData.xy + texData.zw;
    spriteColor = color;
    isBloom = bloom_type.x;
    type = bloom_type.y;
}
