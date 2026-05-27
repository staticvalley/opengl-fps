#pragma once

#include <glad/glad.h>
#include <cinttypes>

class Texture {
public:
	
	/**
	 * @brief constructs texture from raw bytes
	 * @param data raw pixel data
	 * @param width width of image
	 * @param height height of image
	 * @param format representation of image data (`GL_RGB` or `GL_RGBA`)
	 */
	Texture(const uint8_t* data, const int width, const int height, const GLenum format = GL_RGB);

	/**
	 * @brief constructs texture from file
	 * @param filePath full path to image file
	 */
	Texture(const char* filePath);
	
	~Texture();

	// disallow copy
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	/**
	 * @brief binds texture as active
	 * @param slot texture slot to bind to
	 */
	void bind(GLuint slot);
	void unbind();

private:
	GLuint id;
	int width, height, channels;

	void generateTexture(const uint8_t* data, const int width, const int height, const GLenum format);
};