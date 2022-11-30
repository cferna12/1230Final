#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 world_pos;
in vec3 world_norm;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float k_a;

// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float k_d;


// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float k_s;
uniform float shiny;
uniform vec4 world_cam_pos;
uniform vec3 cAmbient;
uniform vec3 cDiffuse;
uniform vec3 cSpecular;

struct Light{
    vec3 color;
    vec3 dir;
    int type;
    vec3 function;
    vec3 pos;
    float angle;
    float penumbra;
};

uniform Light myLights[8];
uniform int total_lights;



void main() {
//    fragColor = vec4(abs(world_norm), 1.0);

    fragColor = vec4(cAmbient*k_a, 1);


    //might have unnecessary normalizes

    for(int i = 0; i < total_lights; i ++){

        vec3 surf_to_light_dir;
        float f_att = 1.f;
        float fall_off_coef = 1.f;

        if(myLights[i].type == 1){ //directional
            surf_to_light_dir = normalize(-myLights[i].dir);
        }

        else{ //spot and point
            surf_to_light_dir = normalize(myLights[i].pos - world_pos);
            float d = distance(myLights[i].pos, world_pos);
            float denom = myLights[i].function[0]+ myLights[i].function[1]*d + myLights[i].function[2]*d*d;
            f_att = min(1.0f, (1/denom));
        }

        if(myLights[i].type == 2){ //if spot, perform penumbra stuff
            float inner_angle = myLights[i].angle - myLights[i].penumbra;
            float x = acos(dot(normalize(myLights[i].dir), -surf_to_light_dir));

            /*If in inner cone, no fallof*/
            if(x <= inner_angle){
                fall_off_coef = 1.f;
            }

            /*If in outer cone*/
            else if(x <= myLights[i].angle){
                float base = (x - inner_angle)/(myLights[i].angle -inner_angle);

                //remember pow has exceptions, might need to check if base is positive
                float fall_off = -2*pow(base,3) + 3*pow(base,2);
                fall_off_coef = 1.f - fall_off;
            }

            /*If not in spotlight, dont include in calculation*/
            else{
                fall_off_coef = 0;
            }
        }

//        surf_to_light_dir = normalize(-myLights[i].dir);

        float dot_prod = dot(surf_to_light_dir, normalize(world_norm));
        dot_prod = dot_prod < 0 ? 0 : dot_prod;

        if(fall_off_coef != 0){
            fragColor += vec4(f_att*fall_off_coef*k_d*dot_prod*(cDiffuse*myLights[i].color), 1);

            // Task 14: add specular component to output color
            vec3 R = vec3(reflect(-surf_to_light_dir, normalize(world_norm)));
            vec3 E =  normalize(vec3(world_cam_pos) - world_pos);
            float dot1 = dot(R, E);
            dot1 = dot1 < 0 ? 0 : dot1;
            dot1 = dot1 > 1 ? 1 : dot1;

            float spec_intens = shiny > 0 ? pow(dot1, shiny) : 0;
            fragColor += vec4(spec_intens*f_att*fall_off_coef*k_s*(myLights[i].color*cSpecular),1);
        }
    }

}



