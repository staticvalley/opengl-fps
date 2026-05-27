#version 460 core

in vec3 v_fragment_position; // world space position
in vec3 v_normal; // world space normal
in vec2 v_uv; // uv coords for texture sampling

out vec4 frag_color;

uniform sampler2D u_texture;

// define types of lights

struct DirectionalLight {
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct PointLight {
    vec3 position;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float cutoff;
    float outer_cutoff;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32

uniform DirectionalLight	u_directional_light;
uniform bool				u_has_directional_light;

uniform PointLight			u_point_lights[MAX_POINT_LIGHTS];
uniform int					u_point_light_count;

uniform SpotLight			u_spot_lights[MAX_SPOT_LIGHTS];
uniform int					u_spot_light_count;

uniform vec3				u_camera_position;

// function prototypes
vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewing_direction);
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragment_position, vec3 viewing_direction);
vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 fragment_position, vec3 viewing_direction);

void main(){

	// get base color from sampling mesh texture
	vec4 texel = texture(u_texture, v_uv);

	// alpha test
	if(texel.a < 0.5)
		discard;

	// get normalized direction from camera to fragment
	vec3 viewing_direction = normalize(u_camera_position - v_fragment_position);

	// normal of surface
	vec3 normal = normalize(v_normal);

	vec3 lighting_factor = vec3(0.0);

	// add directional lighting
	if(u_has_directional_light)
		lighting_factor += calculate_directional_light(u_directional_light, normal, viewing_direction);

	// add point lighting
	for(int i = 0; i < u_point_light_count; i++)
		lighting_factor += calculate_point_light(u_point_lights[i], normal, v_fragment_position, viewing_direction);
	
	// add spot lighting
	for(int i = 0; i < u_spot_light_count; i++)
		lighting_factor += calculate_spot_light(u_spot_lights[i], normal, v_fragment_position, viewing_direction);

	frag_color = vec4(lighting_factor * vec3(texel), texel.a);
}

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewing_direction) {
	
	// get direction of fragment surface towards light
	vec3 light_direction = normalize(-light.direction);

	// get cosine of angle between surface normal and light direction
	// (1): fully lit (0): no light
	float diffuse_weight = max(dot(normal, light_direction), 0.0);

	// get mirrored direction of light
	vec3 reflect_direction = reflect(-light_direction, normal);
	
	// get cosine of angle between viewing direction and reflection and compare
	float specular_weight = pow(max(dot(viewing_direction, reflect_direction), 0.0), 32.0);

	// ambient + diffuse + specular for final light value
	return light.ambient_color + (light.diffuse_color * diffuse_weight) + (light.specular_color * specular_weight);
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragment_position, vec3 viewing_direction) {
	
	// get direction of fragment surface towards light
	vec3 light_direction = normalize(light.position - fragment_position);

	// get cosine of angle between surface normal and light direction
	// (1): fully lit (0): no light
	float diffuse_weight = max(dot(normal, light_direction), 0.0);

	// get mirrored direction of light
	vec3 reflect_direction = reflect(-light_direction, normal);
	
	// get cosine of angle between viewing direction and reflection and compare
	float specular_weight = pow(max(dot(viewing_direction, reflect_direction), 0.0), 32.0);

	// distance to light source from fragment position
	float distance = length(light.position - fragment_position);

	// light "falloff" based on distance
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient = light.ambient_color * attenuation;
    vec3 diffuse = light.diffuse_color * diffuse_weight * attenuation;
    vec3 specular = light.specular_color * specular_weight * attenuation;
    return ambient + diffuse + specular;
}

vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 fragment_position, vec3 viewing_direction) {
	
	// get direction of fragment surface towards light
	vec3 light_direction = normalize(light.position - fragment_position);

	// get cosine of angle between surface normal and light direction
	// (1): fully lit (0): no light
	float diffuse_weight = max(dot(normal, light_direction), 0.0);

	// get mirrored direction of light
	vec3 reflect_direction = reflect(-light_direction, normal);
	
	// get cosine of angle between viewing direction and reflection and compare
	float specular_weight = pow(max(dot(viewing_direction, reflect_direction), 0.0), 32.0);

	// distance to light source from fragment position
	float distance = length(light.position - fragment_position);

	float attenuation = 1.0 / (light.constant + (light.linear * distance) + (light.quadratic * pow(distance, 2)));

	// check fragment surface alignment with center of spotlight cone
	float theta   = dot(light_direction, normalize(-light.direction));


    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

	vec3 ambient  = light.ambient_color * attenuation;
    vec3 diffuse  = light.diffuse_color * diffuse_weight * attenuation * intensity;
    vec3 specular = light.specular_color * specular_weight * attenuation * intensity;
    return ambient + diffuse + specular;
}