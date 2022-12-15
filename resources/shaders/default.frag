#version 330 core
in vec3 world_pos;
in vec3 world_norm;

out vec4 fragColor;

uniform float k_a;
uniform float k_d;

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

uniform int rim_thickness;
uniform float rim_length;
uniform int toon_levels;

uniform bool toon_shading;
uniform bool alternate_rim;

uniform float blend;


void main() {
//    fragColor = vec4(abs(world_norm), 1.0);
    fragColor = vec4(cAmbient*k_a, 1);
    if(toon_shading){
        float toon_scale_factor = 1.0f/toon_levels;
        for(int i = 0; i < total_lights; i++){
            if(myLights[i].type == 1){
                /*Diffuse code*/
                vec3 surf_to_light_dir = normalize(-myLights[i].dir);
                if(cAmbient == vec3(0)){
                    fragColor += vec4(0.25*cDiffuse*k_a, 1);
                }
                vec3 test_ambient = cAmbient == vec3(0) ? 0.5*cDiffuse: cAmbient;
                float dot_prod = dot(surf_to_light_dir, normalize(world_norm));
                fragColor += vec4(test_ambient*k_a*0.5,1);

                float diffuse_factor = 0;

                if(toon_levels == 0){
//                    fragColor += vec4(test_ambient*k_a,1);
//                    float light_intensity = dot_prod > 0 ? 1 : 0; //divide into dark and light bands
//                    dot_prod = dot_prod < 0 ? 0 : dot_prod;
                    float light_intensity = smoothstep(0, blend, dot_prod); //smoothen the change between light and dark
//                    light_intensity = dot_prod;
                    vec3 diffuse = k_d*cDiffuse;
                    fragColor += vec4(diffuse*light_intensity, 1);
//                    if(light_intensity ==  0){
//                        fragColor += vec4(cDiffuse*(toon_scale_factor/4), 1);
//                    }
                }

                ///used for toon levels

                else if(dot_prod > 0){
                    if(toon_levels == 1){
                        diffuse_factor = smoothstep(0, blend, dot_prod);
                    }
                    else{

                        diffuse_factor = ceil(dot_prod * toon_levels) * toon_scale_factor;

                    }


//                    diffuse_factor = smoothstep(diffuse_factor - toon_scale_factor, diffuse_factor-toon_scale_factor+0.2, dot_prod);
                    vec3 diffuse = k_d*cDiffuse;
                    fragColor += vec4(diffuse_factor*diffuse, 1);

//                    }

                }
                else{
//                    fragColor += vec4(cDiffuse*(toon_scale_factor/4), 1);
                    vec3 diffuse = k_d*cDiffuse;
//                    fragColor += vec4(diffuse*(toon_scale_factor/2), 1);
                }

                /*Below is two layer shading code*/



                /*Specular code*/
                vec3 R = vec3(reflect(-surf_to_light_dir, normalize(world_norm)));
                vec3 E =  normalize(vec3(world_cam_pos) - world_pos);
                float dot1 = dot(R, E);
                dot1 = dot1 < 0 ? 0 : dot1;
                dot1 = dot1 > 1 ? 1 : dot1;

                /*intensity to zero if material is shiny? -- square shiny to make more toony*/
                float spec_intens = shiny > 0 ? pow(dot1, shiny*shiny) : 0;

                /*Smoothstep makes specular lighting very toony - need to find way to calibrate correctly*/
                spec_intens = smoothstep(0, 0.01, spec_intens);
                vec3 specular = cSpecular*k_s;
                fragColor += vec4(spec_intens*(myLights[i].color*specular),1);


                if(rim_thickness > 0 && !alternate_rim){
                    float rim_amt = 0.75-(.05*rim_thickness);
                    float rim_dot = 1 - dot(normalize(world_norm), E); //rim intensity
                    dot_prod = dot_prod < 0 ? 0: dot_prod; //clamp if below 0
                    float rim_threshold = rim_length; //controls how far rim extends
                    rim_dot = rim_dot*pow(dot_prod, rim_threshold);
                    rim_dot = smoothstep(rim_amt-0.01, rim_amt + 0.01, rim_dot); //not sure what this does
                    fragColor += vec4(cDiffuse*diffuse_factor*rim_dot*myLights[0].color*k_s,1); //maybe dont mix with light color
                }

                //alternative rim light code
                if(alternate_rim){
                    float rim_amt = 0.75-(.05*rim_thickness);
                    float rim_dot = 1 - dot(normalize(world_norm), E); //rim intensity
                    rim_dot = max(0.0, rim_dot);
                    dot_prod = dot_prod < 0 ? 0: dot_prod; //clamp if below 0
                    float rim_threshold = rim_length+0.75; //controls how far rim extends
                    rim_dot = pow(rim_dot, rim_threshold);
                    fragColor += vec4(cDiffuse*rim_dot*myLights[0].color*k_s,1); //maybe dont mix with light color
                }
                break;
            }

        }

    }


    else{
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
}





