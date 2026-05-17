#pragma once

#include <glad/glad.h>
#include <cinttypes>

class Texture {
public:
	
	// texture from image file
	Texture(const char* filePath);
	
	// texture from raw data
	Texture(uint8_t* data, int width, int height, GLenum format = GL_RGB);
	
	~Texture();

	// disallow copy
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	void bind(GLuint slot);
	void unbind();

private:
	GLuint id;
	int width, height, channels;
};