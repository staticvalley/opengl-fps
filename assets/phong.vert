#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec4 o_world_normal;
out vec4 o_fragment_position;
out vec4 o_color;

void main(){
	// world space normal 
    o_world_normal = u_model * normal;
	// world space position
	o_fragment_position = u_model * position;
	// get color
	o_color = color
	// P*V*M*p
	gl_Position = u_projection * u_view * u_model * position;
}