#version 330 core

in vec4 o_world_normal;
in vec4 o_fragment_position;
in vec4 o_color;

out vec4 frag_color;

void main(){

	// base constants
	float u_ambient_level = 0.8;
	float u_shininess = 40;

	// compute ambient lighting 
	vec4 ambient = (o_color * u_ambient_level);

	// get distance between lightsource and fragment
	float distance = length(u_light_position - o_fragment_position);
		
	// my custom attenuation func
	// TODO: remove first attempt if other works:
	//float attenuation = 1.0 / pow(distance*u_light_strength, 2);
	// slides have min(1/c1+(c2*dl)+(c3*dl^2))
	float attenuation = min(1.0 / (1.0 + (u_light_strength * pow(distance, 2.0))), 1.0);

	// compute diffuse lighting prerequisites
	vec4 N = normalize(o_world_normal); // surface normal
	vec4 L = normalize(u_light_position - o_fragment_position); // light direction vector

	// add diffuse from light to the final diffuse value
	vec4 diffuse = o_color * clamp(dot(N, L), 0.0, 1.0) * attenuation;

	// compute specular lighting prerequisites
	vec4 V = normalize(u_view_position - o_fragment_position);
	vec4 R = -L + (2.0 * dot(N, L) * N);

	// compute specular lighting
	float spec = pow(clamp(dot(V,R), 0.0, 1.0), u_shininess);

	// add specular from some light to the final specular value
	vec4 specular = o_color * spec * attenuation;

	// add all sources
	frag_color = ambient + diffuse + specular;
}