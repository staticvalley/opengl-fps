#include <Collision.h>

bool AABB::intersects(const AABB& otherBox) {
	/* for each axis:
	 *	min.axis <= other.max.axis
	 *  max.axis >= other.min.axis
	 */
	return	min.x <= otherBox.max.x && max.x >= otherBox.min.x &&
			min.y <= otherBox.max.y && max.y >= otherBox.min.y &&
			min.z <= otherBox.max.z && max.z >= otherBox.min.z;
}