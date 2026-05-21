#pragma once

#include <BSPFile.hpp>
#include <WADFile.hpp>

#include <Mesh.hpp>
#include <Shader.hpp>
#include <glad/glad.h>
#include <vector>
#include <unordered_map>

#define ENGINE_WAD_DIR "assets/game/"

// bsp files are in inches, engine is in meters
#define BSP_TO_ENGINE_UNIT_CONVERSION 0.025f

struct BSPSurface {
	Mesh* mesh;
	std::shared_ptr<Texture> texture;
};

class BSPMap {
public:
	BSPMap(const char* bspPath);
	~BSPMap();

	void draw(Shader& shader);

	// given some line, check where it intersects the clipping hull
	int traceHullSegment(
		const int nodeIndex, 
		const glm::vec3& segmentStart, 
		const glm::vec3& segmentEnd, 
		glm::vec3& outNormal
	);
	
	BSPFile bsp;
	std::unordered_map<std::string, WADTexture> wadTextureMap;

	std::vector<BSPSurface> surfaces;
	Texture* nullTexture = nullptr;

private:

	// construct texture-binded surfaces for each face in world model
	void buildSurfaces();

	// creates shared pointer to texture from given bsp entry
	std::shared_ptr<Texture> buildTexture(const BSPTexture& bspTexture);

	// build opengl mesh from bsp face data
	Mesh* buildFaceMesh(const BSPFace& face);
};