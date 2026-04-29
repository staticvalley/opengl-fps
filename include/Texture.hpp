#pragma once

#include <glad/glad.h>

class Texture {
public:
	
	Texture(const char* filePath);
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