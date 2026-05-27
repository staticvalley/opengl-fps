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
	glm::vec3 groundCheck = origin + glm::vec3(0, PM_GROUNDCHECK_EPSILON, 0);
	int groundResult = traceHull(origin, groundCheck, map, groundNormal, BSP_HULL_STANDING);
	if (groundResult != BSP_CONTENTS_EMPTY)
		isGrounded = true;

	//std::println("pos.y:{:.3f} groundCheck.y:{:.3f} groundResult:{}", position.y, groundCheck.y, groundResult);

	if (!isGrounded)
		velocity.y += PM_GRAVITY * dt;
	else if (velocity.y < 0.0f)
		velocity.y = 0.0f;

	glm::vec3 collidedNormal(0.0f);
        glm::vec3 wishDestination = origin + velocity * dt;

	int traceResult =
            traceHull(origin, wishDestination, map, collidedNormal, BSP_HULL_STANDING);

	//std::println("traceResult: {}", traceResult);

	if (traceResult == BSP_CONTENTS_EMPTY) {
		origin = wishDestination;

	} else {
		// not just colliding with the ground
		if (collidedNormal.y < 0.7f && collidedNormal.y > -0.7) {
			
			//std::println("wall");
			//std::println("[{} {} {}]", collidedNormal.x, collidedNormal.y, collidedNormal.z);

			glm::vec3 stepUpStart = origin + glm::vec3(0, PM_STEPUP_EPSILON, 0);
			glm::vec3 stepUpDestination = stepUpStart + glm::vec3(velocity.x * dt, 0, velocity.z * dt);

			glm::vec3 stepNormal(0.0f);
			int stepTraceResult = traceHull(stepUpStart, stepUpDestination, map, stepNormal, BSP_HULL_STANDING);

			if (stepTraceResult == BSP_CONTENTS_EMPTY) {
				glm::vec3 dropDestination = stepUpDestination + glm::vec3(0, -PM_STEPUP_EPSILON, 0);
				glm::vec3 dropNormal(0.0f);
				int dropResult = traceHull(stepUpDestination, dropDestination, map, dropNormal, BSP_HULL_STANDING);
				origin = (dropResult == BSP_CONTENTS_EMPTY) ? dropDestination : stepUpDestination;
				//origin.y -= (PM_PLAYER_HEIGHT / 2);
			}
			else {
				velocity = clipVelocity(velocity, collidedNormal, 1.0f);
                          wishDestination = origin + velocity * dt;
				glm::vec3 newNormal(0.0f);
				if (traceHull(origin, wishDestination, map, newNormal, BSP_HULL_STANDING) == BSP_CONTENTS_EMPTY) {
					origin = wishDestination;
					//origin.y -= (PM_PLAYER_HEIGHT / 2);
				}
					
			}	
		}else if (collidedNormal.y <= -0.7) {
			// hit ceiling
			velocity.y = 0;
			std::println("ceiling");
		} else {
			// hit floor
			if (velocity.y < 0.0f) velocity.y = 0.0f;
			std::println("floor");
		}
	}
}

void PlayerMovement::noclipMove(float dt) {
	origin += velocity * dt;
}

glm::vec3 PlayerMovement::clipVelocity(const glm::vec3& velocity, const glm::vec3 normal, float overBounce) {
	return velocity - normal * (glm::dot(velocity, normal) * overBounce);
}

int PlayerMovement::traceHull(const glm::vec3& start, const glm::vec3& end, BSPMap& map, glm::vec3& outNormal, uint8_t hullType) {
	if (hullType > 3)
		return BSP_CONTENTS_SOLID;
	return  map.traceHullSegment(map.bsp.models()[0].iHeadnodes[hullType], start / BSP_TO_ENGINE_UNIT_CONVERSION, end / BSP_TO_ENGINE_UNIT_CONVERSION, outNormal);
}


