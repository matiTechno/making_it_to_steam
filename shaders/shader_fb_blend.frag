#version 330

out vec4 color;

in vec2 texCoord;

uniform sampler2D base;
uniform sampler2D blur;

uniform bool is_tone_mapping;

void main()
{
    color = texture(base, texCoord) + vec4(texture(blur, texCoord).rgb, 0);
}
