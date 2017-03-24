#version 330

out vec4 color;

in vec2 texCoord;

uniform sampler2D sampl;

uniform float wave_vel;
uniform float wave_length;
uniform float time;
uniform ivec4 scene_coords;
uniform float Pi = 3.14159265359;

void main()
{
    float freq = wave_vel / wave_length;
    vec2 scene_center = scene_coords.xy + scene_coords.zw / 2.0;
    float r = length(scene_center - gl_FragCoord.xy);
    float h = sin(2.0 * Pi / wave_length * r - 2.0 * Pi * freq * time) / 2.0 + 0.5;
    color = vec4(0, 0, 0, h);
}
