R"(
#version 330

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec4 pos_size;
layout (location = 2) in vec4 color;
layout (location = 3) in vec4 in_texCoords;
layout (location = 4) in int isBloom;

uniform mat4 projection;

// this is actual size of a texture
// particles are rendered in batch so for every
// particle gl format texCoords are calculated in shader
uniform vec2 texSize;

out vec2 texCoord;
out vec4 spriteColor;
flat out int out_isBloom;

void main()
{
    gl_Position = projection * vec4(vertex.xy * pos_size.zw + pos_size.xy, 0, 1);
    texCoord = vertex.zw * in_texCoords.zw / texSize + in_texCoords.xy / texSize;
    spriteColor = color;
    out_isBloom = isBloom;
}
)"
