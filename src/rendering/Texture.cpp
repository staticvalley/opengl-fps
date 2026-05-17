
#define STB_IMAGE_IMPLEMENTATION

#include "../libs/stb/stb_image.h"
#include <Texture.hpp>
#include <SDL3/SDL.h>

Texture::Texture(const char* filePath) {
	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// uv sampling wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// scaled sampling parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// flip image so y=0 is bottom of image
	stbi_set_flip_vertically_on_load(true);

	// read image
	unsigned char* textureData = stbi_load(filePath, &width, &height, &channels, 0);
	if (textureData) {
		GLenum textureFormat = channels == 4 ? GL_RGBA : GL_RGB;
		// convert to opengl texture format
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			textureFormat,
			width,
			height,
			0,
			textureFormat,
			GL_UNSIGNED_BYTE,
			textureData
		);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		SDL_Log("failed to load texture file \"%s\"", filePath);
	}
	stbi_image_free(textureData);
}

Texture::Texture(uint8_t* data, int width, int height, GLenum format) {
	
	glGenTextures(1, &id); 
	glBindTexture(GL_TEXTURE_2D, id);
	
	// uv sampling wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// scaled sampling parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		width,
		height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data
	);
	glGenerateMipmap(GL_TEXTURE_2D);
}


Texture::~Texture() {
	glDeleteTextures(1, &id);
}

void Texture::bind(GLuint slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}