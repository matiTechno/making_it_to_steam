#version 330

out vec4 color;

in vec2 texCoord;

uniform sampler2D sampl;

uniform bool is_tone_mapping;

void main()
{
    vec3 color1 = texture(sampl, texCoord).rgb;

    // gamma correction
    color1 = pow(color1, vec3(1.0 / 2.2));

    color = vec4(color1, 1);
}
