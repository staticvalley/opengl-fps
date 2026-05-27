#pragma once 

#include <glm/glm.hpp>

/**
 * @brief holds spacial, normal and texture data for a vertex
 */
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};