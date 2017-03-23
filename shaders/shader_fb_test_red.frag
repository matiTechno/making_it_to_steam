#version 330

out vec4 color;

in vec2 texCoord;

uniform sampler2D sampl;

void main()
{
    color = texture(sampl, texCoord) * vec4(1, 0.6, 0.6, 1);
}
