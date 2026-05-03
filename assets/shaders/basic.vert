#version 460 core

// inputs from vbo
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

// outputs to fragment shader
out vec3 v_fragment_position; // world space position
out vec3 v_normal; // world space normal
out vec2 v_uv; // uv coords for texture sampling

// mvp matrices
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    vec4 world_position = u_model * vec4(a_position, 1.0);
    
    v_fragment_position = vec3(world_position);
    v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    v_uv = a_uv;

    gl_Position = u_projection * u_view * world_position;
}