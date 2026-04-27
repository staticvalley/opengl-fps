#pragma once

#include "include/Model.hpp"
#include "include/Mesh.hpp"
#include "include/Shader.hpp"

#include <glm/glm.hpp>
#include <vector>

Model::Model()
	: position(0.0f),
	  rotation(0.0f),
	  scale(1.0f)
{}

void Model::addMesh(Mesh mesh) {
	meshList.push_back(std::move(mesh)); // move ownership of mesh to Model
}

void Model::draw(Shader& shader) {
	shader.setUniformMat4("u_model", getModelMatrix());
	for (Mesh& mesh : meshList)
		mesh.draw();
}

glm::mat4 Model::getModelMatrix() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	model = glm::scale(model, scale);
	return model;
}
