#include "../include/Material.hpp"

void Material::apply(Shader& shader) {
	if (diffuseTexture) {
		diffuseTexture->bind(0);
		shader.setUniformInt1("u_texture", 0);
	}
}
