#pragma once

#include <BSPMap.hpp>
#include <glm/glm.hpp>

#define PM_MOVEMENT_SPEED		10.0f
#define PM_JUMP_FORCE			4.5f
#define PM_GRAVITY				-9.8f

// 64 hammer units
#define PM_EYEHEIGHT			(64.0f * BSP_TO_ENGINE_UNIT_CONVERSION)

// 28 hammer units
#define PM_EYEHEIGHT_CROUCH		(28.0f * BSP_TO_ENGINE_UNIT_CONVERSION)

// 18 hammer units
#define PM_STEPUP_EPSILON		(18.0f * BSP_TO_ENGINE_UNIT_CONVERSION)
#define PM_GROUNDCHECK_EPSILON	-0.05f

enum class MovementType : uint8_t {
	WALK = 0,
	NOCLIP = 1
};

struct PlayerMovement {

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 inputMovement; // movement input through keyboard

	bool jumpPressed = false;
	bool crouchPressed = false;

	bool isGrounded = false;
	bool inWater = false;

	MovementType moveType = MovementType::WALK; // walk, noclip

	void update(float dt, const glm::vec3& wishDir, BSPMap& map);

private:
	void walkMove(float dt,BSPMap& map);
	void noclipMove(float dt);
	glm::vec3 clipVelocity(const glm::vec3& velocity, const glm::vec3 collidedNormal, float overBounce);
	int traceHull(const glm::vec3& start, const glm::vec3& end, BSPMap& map, glm::vec3& outNormal);
};