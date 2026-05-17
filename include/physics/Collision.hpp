#pragma once

#include <glm/glm.hpp>

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	// check if box intersects with other box
	bool intersects(const AABB& otherBox);

	// create bounding box given position and box size
	static AABB createFromPoint(const glm::vec3& position, const glm::vec3& boxSize);
};