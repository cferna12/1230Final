#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 obj_pos;

//normals are located in vao ag
layout(location = 1) in vec3 obj_norm;

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec3 world_pos;
out vec3 world_norm;

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 m_model;
uniform mat3 m_inverse_transpose;

// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 m_view;
uniform mat4 m_proj;

uniform bool using_obj_file;

void main() {
    if(using_obj_file){
        world_norm = obj_norm;
        gl_Position = vec4(obj_pos,1);
    }
    world_pos = vec3((m_model)*vec4(obj_pos,1));
    world_norm = normalize(m_inverse_transpose*normalize(obj_norm));
    gl_Position = m_proj*m_view*m_model*vec4(obj_pos,1.0);
}
