// basic.frag
#version 460 core

in vec2 v_uv;
out vec4 fragment_color;

uniform sampler2D u_texture;

void main() {
    fragment_color = texture(u_texture, v_uv);
}