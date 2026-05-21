#include <PlayerMovement.hpp>

#include <BSPFile.hpp>
#include <print>

void PlayerMovement::update(float dt, const glm::vec3& wishDir, BSPMap& map) {

	// apply movement speed
	velocity.x = wishDir.x * PM_MOVEMENT_SPEED;
	velocity.z = wishDir.z * PM_MOVEMENT_SPEED;

	switch (moveType) {
	case MovementType::WALK:
		// add jump velocity
		if (jumpPressed && isGrounded) {
			velocity.y = PM_JUMP_FORCE;
		}
		// trace hull for collision
		walkMove(dt, map); 
		break;
	case MovementType::NOCLIP:
		// add float velocity
		velocity.y = wishDir.y * PM_MOVEMENT_SPEED;
		// apply move
		noclipMove(dt);
		break;
	default:
		break;
	}
}

void PlayerMovement::walkMove(float dt, BSPMap& map) {

	isGrounded = false;

	// check that we are on ground
	glm::vec3 groundNormal(0.0f);
	glm::vec3 groundCheck = position + glm::vec3(0, PM_GROUNDCHECK_EPSILON, 0);
	if (traceHull(position, groundCheck, map, groundNormal) != BSP_CONTENTS_EMPTY)
		isGrounded = true;

	if (!isGrounded)
		velocity.y += PM_GRAVITY * dt;
	else if (velocity.y < 0.0f)
		velocity.y = 0.0f;

	glm::vec3 collidedNormal(0.0f);
	glm::vec3 wishDestination = position + velocity * dt;

	int traceResult = traceHull(position, wishDestination, map, collidedNormal);

	//std::println("traceResult: {}", traceResult);

	if (traceResult == BSP_CONTENTS_EMPTY) {
		position = wishDestination;
	} else {
		// not just colliding with the ground
		if (collidedNormal.y < 0.7f) {

			glm::vec3 stepUpStart = position + glm::vec3(0, PM_STEPUP_EPSILON, 0);
			glm::vec3 stepUpDestination = stepUpStart + glm::vec3(velocity.x * dt, 0, velocity.z * dt);

			glm::vec3 stepNormal(0.0f);
			int stepTraceResult = traceHull(stepUpStart, stepUpDestination, map, stepNormal);

			if (stepTraceResult == BSP_CONTENTS_EMPTY) {
				glm::vec3 dropDestination = stepUpDestination + glm::vec3(0, -PM_STEPUP_EPSILON, 0);
				glm::vec3 dropNormal(0.0f);
				int dropResult = traceHull(stepUpDestination, dropDestination, map, dropNormal);
				position = (dropResult == BSP_CONTENTS_EMPTY) ? dropDestination : stepUpDestination;
			} else {
				velocity = clipVelocity(velocity, collidedNormal, 1.0f);
				wishDestination = position + velocity * dt;
				if (traceHull(position, wishDestination, map, collidedNormal) == BSP_CONTENTS_EMPTY)
					position = wishDestination;
			}		
		} else {
			// hit floor
			if (velocity.y < 0.0f) velocity.y = 0.0f;
		}
	}
}

void PlayerMovement::noclipMove(float dt) {
	position += velocity * dt;
}

glm::vec3 PlayerMovement::clipVelocity(const glm::vec3& velocity, const glm::vec3 normal, float overBounce) {
	return velocity - normal * (glm::dot(velocity, normal) * overBounce);
}

int PlayerMovement::traceHull(const glm::vec3& start, const glm::vec3& end, BSPMap& map, glm::vec3& outNormal) {
	return  map.traceHullSegment(map.bsp.models()[0].iHeadnodes[1], start, end, outNormal);
}


