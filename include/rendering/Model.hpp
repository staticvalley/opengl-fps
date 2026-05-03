#pragma once

#include <Mesh.hpp>
#include <Shader.hpp>

#include <glm/glm.hpp>
#include <vector>

class Model {
public:

	Model();

	// note: this function moves mesh data ownership from argument into Model
	void addMesh(Mesh mesh);

	void setTexture(Texture* texture);

	void draw(Shader& shader);

	static Model* loadOBJ(const char* filePath);

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 getModelMatrix();

private:

	// vector of meshes for model
	std::vector<Mesh> meshList;

};