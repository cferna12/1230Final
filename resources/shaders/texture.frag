#version 330 core
in vec2 UV;

uniform sampler2D m_texture;

uniform bool invert;
uniform bool grayscale;
uniform bool kernel_based;
uniform bool sharpen;

uniform mat3 k;
uniform int width;
uniform int height;

out vec4 fragColor;

void main()
{
    fragColor = texture(m_texture, UV);

    if(invert){
        fragColor = 1 - fragColor;
    }

    if(grayscale){
        float sum = fragColor[0]+fragColor[1]+fragColor[2];
        fragColor = vec4(vec3(sum/3.f), 1);
    }


    if(kernel_based){
        fragColor = vec4(0);
        float UV_pixel_x = 1.f/(width);
        float UV_pixel_y = 1.f/(height);

        for(int i = -2; i <= 2; i++){
            for(int j = -2; j <=2; j++){
                vec2 neigh = vec2(UV[0]+UV_pixel_x*i, UV[1]+UV_pixel_y*j);
                fragColor += texture(m_texture, neigh);
            }
        }
        fragColor = fragColor/25.f;
    }

    if(sharpen){
        fragColor = vec4(0);
        float UV_pixel_x = 1.f/(width);
        float UV_pixel_y = 1.f/(height);
        for(int i = -1; i <=1 ; i++){
            for(int j = -1; j <= 1; j++){
                vec2 neigh = vec2(UV[0]+UV_pixel_x*i, UV[1]+UV_pixel_y*j);
                fragColor += k[j+1][i+1]*texture(m_texture, neigh);

            }
        }
        fragColor = fragColor/9.f;
    }

}
