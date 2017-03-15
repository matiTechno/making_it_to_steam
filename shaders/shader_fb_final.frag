#version 330

out vec4 color;

in vec2 texCoord;

uniform sampler2D sampl;

uniform bool is_tone_mapping;

void main()
{
    vec3 hdr_color = texture(sampl, texCoord).rgb;

    // tone mapping
    if(is_tone_mapping)
    {
        hdr_color = hdr_color / (hdr_color + vec3(1));
    }

    // gamma correction
    hdr_color = pow(hdr_color, vec3(1.0 / 2.2));

    color = vec4(hdr_color, 1);
}
