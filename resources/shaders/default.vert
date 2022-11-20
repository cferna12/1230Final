#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 obj_pos;

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec3 world_pos;
out vec3 world_norm;

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 m_model;

// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 m_view;
uniform mat4 m_proj;


void main() {
    // Task 8: compute the world-space position and normal, then pass them to
    //         the fragment shader using the variables created in task 5
    world_pos = mat3(m_model)*obj_pos;

    //probs shouldnt calculate inverse here

    //not correct normal
    world_norm = transpose(inverse(mat3(m_model)))*normalize(obj_pos);

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    // Task 9: set gl_Position to the object space position transformed to clip space
    gl_Position = m_proj*m_view*m_model*vec4(obj_pos,1.0);

}
