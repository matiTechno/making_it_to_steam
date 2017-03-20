#version 330

out vec4 color;

in vec2 texCoord;

uniform sampler2D sampl;

void main()
{
    color = texture(sampl, texCoord);

    // gamma correction
    color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}
