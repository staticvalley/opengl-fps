#pragma once

#include "Vertex.hpp"
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
	
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	~Mesh();

	// disallow copy, must transfer ownership
	// i do this so we dont get double frees on vao/vbo/ebo data
	// ideally, there should only be one owner at one time
	// https://stackoverflow.com/questions/33776697/deleting-copy-constructors-and-copy-assignment-operators-which-of-them-are-esse
	Mesh(const Mesh&) = delete; // no copy constructor
	Mesh& operator=(const Mesh&) = delete; // no copy assignment
	Mesh(Mesh&& other) noexcept; // create move constructor
	Mesh& operator=(Mesh&& other) noexcept; // create move assignment

	void draw();

private:
	// id to array object
	GLuint vao;

	// id to buffer object
	GLuint vbo, ebo;
	
	// index count
	GLsizei count;
};