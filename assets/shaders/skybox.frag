#version 330 core

in vec3 texture_coord;

uniform samplerCube u_skybox;

out vec4 frag_color;

void main() {
    frag_color = texture(u_skybox, texture_coord);
}