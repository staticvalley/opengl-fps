#version 460 core
layout(location = 0) in vec3 a_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

flat out vec3 v_color;

void main() {
    v_color = vec3(
                step(0.001, a_position.x),
                step(0.001, a_position.y),
                step(0.001, a_position.z)
              );
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}