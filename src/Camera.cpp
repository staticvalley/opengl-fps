#pragma once

#include "../include/Camera.hpp"
#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 startPosition, GLfloat fov, GLfloat aspectRatio)
	: position(startPosition),
	  fov(fov),
	  aspectRatio(aspectRatio),
	  // view presets
	  yaw(-90.0f),
	  pitch(0.0f),
	  // control presets
	  mouseSensitivity(0.3f),
	  movementSpeed(1.0f),
	  // basic starting camera relative vectors
	  forward(0.0f, 0.0f, -1.0f),
	  up(0.0f, 1.0f, 0.0f),
	  right(1.0f, 0.0f, 0.0f)
{
	calcuateRelativeVectors();
}

Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix() {
	// lookat args (camera position, target position, up)
	return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera::getProjectionMatrix() {
	/*
	 * glm::perspective args
	 * - fov
	 * - aspect ratio
	 * - near frustum cutoff
	 * - far frustum cutoff
	 */
	return glm::perspective(
		glm::radians(fov),
		aspectRatio,
		0.1f,
		100.0f
	);
}

void Camera::calcuateRelativeVectors() {
	/*
	 * forward vector
	 * eulers angles calculation
	 */
	forward = glm::normalize(
		glm::vec3(
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		)
	);

	// right vector (cross product of camera forward and world up (0, 1, 0))
	right = glm::normalize(
		glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))
	);

	// up vector (cross product of camera forward and camera right)
	up = glm::normalize(
		glm::cross(forward, right)
	);
}

void Camera::processKeyboard(float dt) {
	
	float velocity = movementSpeed * dt;

	// get keyboard inputs
	const bool* keyInputs = SDL_GetKeyboardState(nullptr);

	if (keyInputs[SDL_SCANCODE_W]) position += forward * velocity;
	if (keyInputs[SDL_SCANCODE_S]) position -= forward * velocity;
	if (keyInputs[SDL_SCANCODE_A]) position += right * velocity;
	if (keyInputs[SDL_SCANCODE_D]) position -= right * velocity;
}

void Camera::processMouse(float x, float y) {

	// sdl3's xrel event value is positive when moving right, right is negative in world space, so negation is needed
	yaw += -x * mouseSensitivity;
	pitch += y * mouseSensitivity;

	// clamp pitch so you cant roll over top
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	// recalculate forward/right/up vectors now that view has changed
	calcuateRelativeVectors();
}
