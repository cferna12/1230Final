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
uniform vec4 world_light_pos;

// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float k_s;
uniform float shiny;
uniform vec4 world_cam_pos;

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    fragColor = vec4(1.0);

    // Task 11: set your output color to the absolute value of your world-space normals,
    //          to make sure your normals are correct.
    fragColor = vec4(abs(world_norm), 1.0);

    // Task 12: add ambient component to output color
    fragColor = vec4(k_a, k_a, k_a, 1);

    // Task 13: add diffuse component to output color
    vec3 surf_to_light_dir = normalize(vec3(world_light_pos) - world_pos);
    float diff_intense = dot(surf_to_light_dir, normalize(world_norm));
    diff_intense = diff_intense < 0 ? 0 : diff_intense;

    fragColor += k_d*diff_intense;
    fragColor[3] = 1;

    // Task 14: add specular component to output color
//    reflect()
    vec3 R = vec3(reflect(vec3(normalize(world_pos - vec3(world_light_pos))), normalize(world_norm)));
    vec3 E =  normalize(vec3(world_cam_pos) - world_pos);
    float dot = dot(R, E);
    dot = dot < 0 ? 0 : dot;
    dot = dot > 1 ? 1 : dot;
    float spec_intens = pow(dot, shiny);
    fragColor += k_s*spec_intens;
    fragColor[3] = 1;


}

