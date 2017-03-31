R"(
#version 330

layout (location = 0) in vec4 vertex;

uniform mat4 projection;

uniform mat4 model;
uniform vec2 texSize;
uniform vec2 texShift;

out vec2 texCoord;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0, 1);
    texCoord = vertex.zw * texSize + texShift;
}
)"
