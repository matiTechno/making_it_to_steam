#version 330

#define kernel_size 5

out vec4 color;

in vec2 texCoord;

uniform bool horizontal;

uniform sampler2D sampl;

uniform float weights[kernel_size] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texel_size = 1.0 / textureSize(sampl, 0);

        vec3 color1 = texture(sampl, texCoord).rgb * weights[0];

        if(horizontal)
        {
            for(int i = 1; i < kernel_size; ++i)
            {
                color1 += texture(sampl, texCoord + vec2(i * texel_size.x, 0)).rgb * weights[i];
                color1 += texture(sampl, texCoord - vec2(i * texel_size.x, 0)).rgb * weights[i];
            }
        }
        else
        {
            for(int i = 1; i < kernel_size; ++i)
            {
                color1 += texture(sampl, texCoord + vec2(0, i * texel_size.y)).rgb * weights[i];
                color1 += texture(sampl, texCoord - vec2(0, i * texel_size.y)).rgb * weights[i];
            }
        }
        color = vec4(pow(color1, vec3(1 / 1.05)), 1);
       //color = vec4(color1 , 1);

//    color = vec4(0, 0, 0, 0);
//    vec3 vec_zero = vec3(0);
//    vec3 color_temp = texture(sampl, texCoord).rgb * weights[0];
//    if(color_temp != vec_zero)
//        color.rgb += color_temp;

//    if(horizontal)
//    {
//        for(int i = 1; i < kernel_size; ++i)
//        {
//            vec3 colortemp1 = texture(sampl, texCoord + vec2(i * texel_size.x, 0)).rgb * weights[i];
//            if(colortemp1 != vec_zero)
//                color.rgb += colortemp1;
//            vec3 colortemp2 = texture(sampl, texCoord - vec2(i * texel_size.x, 0)).rgb * weights[i];
//            if(colortemp2 != vec_zero)
//                color.rgb += colortemp2;
//        }
//    }
//    else
//    {
//        for(int i = 1; i < kernel_size; ++i)
//        {
//            vec3 colortemp1 = texture(sampl, texCoord + vec2(0, i * texel_size.y)).rgb * weights[i];
//            if(colortemp1 != vec_zero)
//                color.rgb += colortemp1;
//            vec3 colortemp2 = texture(sampl, texCoord - vec2(0, i * texel_size.y)).rgb * weights[i];
//            if(colortemp2 != vec_zero)
//                color.rgb += colortemp2;
//        }
//    }
//    if(color.rgb != vec_zero)
//        color.a = 1;
}
