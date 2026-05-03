#pragma once

#include <Texture.hpp>
#include <Shader.hpp>

class Material {
public:
	Texture* diffuseTexture = nullptr;
	
	// applies active texture for drawing meshes
	void apply(Shader& shader);
};