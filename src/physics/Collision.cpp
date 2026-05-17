#include <Collision.hpp>

bool AABB::intersects(const AABB& otherBox) {
	/* for each axis:
	 *	min.axis <= other.max.axis
	 *  max.axis >= other.min.axis
	 */
	return	min.x <= otherBox.max.x && max.x >= otherBox.min.x &&
			min.y <= otherBox.max.y && max.y >= otherBox.min.y &&
			min.z <= otherBox.max.z && max.z >= otherBox.min.z;
}

AABB AABB::createFromPoint(const glm::vec3& position, const glm::vec3& boxSize) {
	glm::vec3 half = boxSize * 0.5f;
	return { position - half, position + half };
}