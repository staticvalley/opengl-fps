#include <BSPMap.hpp>
#include <BSPFile.hpp>
#include <WADFile.hpp>
#include <string>
#include <SDL3/SDL.h>

BSPMap::BSPMap(const char* bspPath)
{
	if (!bsp.load(bspPath))
		return;
	
	// get WAD paths
	std::vector<std::string> wadPaths = bsp.requiredWADs();

	// read all WAD paths needed by bsp file
	for (std::string wadName : wadPaths) {
		WADFile wad;
		std::string wadPath = ENGINE_WAD_DIR + wadName;
		if (!wad.load(wadPath.c_str())) {
			SDL_Log(std::format("BSPMap: missing \"{}\"", wadPath).c_str());
			continue;
		}
		wadTextureMap.merge(wad.takeTextures());
	}

	buildSurfaces();
}

BSPMap::~BSPMap() {
	for (BSPSurface& surface : surfaces) {
		delete surface.mesh;
	}
	delete nullTexture;
}

void BSPMap::draw(Shader& shader) {

	// BSP geometry is already in world space, use identity model matrix
	shader.setUniformMat4("u_model", glm::mat4(1.0f));

	for (BSPSurface& surface : surfaces) {
		if (surface.texture) {
			surface.texture->bind(0);
			shader.setUniformInt1("u_texture", 0);
		}
		else if(nullTexture) {
			nullTexture->bind(0);
			shader.setUniformInt1("u_texture", 0);
		}		
		surface.mesh->draw(shader);
	}
}

void BSPMap::buildSurfaces() {
	
	// model 0 is world geometry model
	const BSPModel& worldModel = bsp.models()[0];

	// make cache for texture pointers
	std::unordered_map<int, std::shared_ptr<Texture>> textureCache;

	// create surface for each face in model
	for (int i = 0; i < worldModel.nFaces; i++) {

		// get given face and texture info for face
		const BSPFace& face = bsp.faces()[worldModel.iFirstFace + i];
		const BSPTextureInfo& texInfo = bsp.textureInfo()[face.iTextureInfo];

		// skip sky and special faces
		if (texInfo.nFlags & 0x1) continue;  // SKY flag

		// get texture referenced by texture info
		const uint32_t textureIndex = texInfo.iMiptex;
		const BSPTexture currentTexture = bsp.textures()[texInfo.iMiptex];

		// add texture to cache
		if (!textureCache.contains(textureIndex)) {
			// if texture referenced is in an external wad
			if (currentTexture.external) {
				// search for texture name in wad map
				auto search = wadTextureMap.find(currentTexture.name);
				if (search != wadTextureMap.end()) {
					// found, build texture
					WADTexture referencedTexture = wadTextureMap[currentTexture.name];
					textureCache[textureIndex] = buildTexture(referencedTexture);
				} else {
					// texture not found
					textureCache[textureIndex] = nullptr;
				}
			} else {
				// texture in bsp file 
				textureCache[textureIndex] = buildTexture(bsp.textures()[textureIndex]);
			}
		}

		BSPSurface surface;
		surface.mesh = buildFaceMesh(face);
		surface.texture = textureCache[textureIndex];
		surfaces.push_back(surface);
	}
}

std::shared_ptr<Texture> BSPMap::buildTexture(const BSPTexture& bspTexture) {
		
	if (bspTexture.pixels.empty()) return nullptr;

	// convert paletted pixels to rgb
	std::vector<uint8_t> rgb(bspTexture.width * bspTexture.height * 3);
	for (int p = 0; p < bspTexture.width * bspTexture.height; p++) {
		uint8_t paletteIndex = bspTexture.pixels[p];
		rgb[p * 3 + 0] = bspTexture.palette[paletteIndex * 3 + 0];
		rgb[p * 3 + 1] = bspTexture.palette[paletteIndex * 3 + 1];
		rgb[p * 3 + 2] = bspTexture.palette[paletteIndex * 3 + 2];
	}

	return std::shared_ptr<Texture>(new Texture(rgb.data(), bspTexture.width, bspTexture.height));
}

Mesh* BSPMap::buildFaceMesh(const BSPFace& face) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	const BSPTextureInfo& texInfo = bsp.textureInfo()[face.iTextureInfo];
	const BSPTexture& texture = bsp.textures()[texInfo.iMiptex];

	// collect vertices in order
	std::vector<glm::vec3> faceVertices;
	for (int i = 0; i < face.nEdges; i++) {
		
		BSPSurfEdge surfEdge = bsp.surfEdges()[face.iFirstEdge + i];
		BSPEdge edge;
		BSPVertex bspVertex;

		/*
			idk how tf these work or why they are needed, but following the valvesoftware wiki:

			... If the value of the surfedge is positive, the first vertex of the edge is used as vertex for rendering 
			the face, otherwise, the value is multiplied by -1 and the second vertex of the indexed edge is used ...
		*/
		if (surfEdge >= 0)
			edge = bsp.edges()[surfEdge];
		else
			edge = bsp.edges()[-surfEdge];

		if (surfEdge >= 0)
			bspVertex = bsp.vertices()[edge.iVertex[0]];
		else
			bspVertex = bsp.vertices()[edge.iVertex[1]];

		// goldsrc bsp's have +z as up, converting vertex to opengl
		faceVertices.push_back(
			glm::vec3(bspVertex.x, bspVertex.z, -bspVertex.y) * BSP_TO_ENGINE_UNIT_CONVERSION
		);
	}

	const BSPPlane& plane = bsp.planes()[face.iPlane];

	// get normal of face plane in opengl directions
	glm::vec3 normal = glm::vec3(plane.vNormal.x, plane.vNormal.z, -plane.vNormal.y);
	
	// flip orientation
	if (face.nPlaneSide) normal = -normal;

	// set all data for single vertex
	for (glm::vec3& position : faceVertices) {
		Vertex vertex;
		vertex.position = position;
		vertex.normal = normal;

		// texture access vectors
		glm::vec3 s = glm::vec3(texInfo.vS.x, texInfo.vS.z, -texInfo.vS.y);
		glm::vec3 t = glm::vec3(texInfo.vT.x, texInfo.vT.z, -texInfo.vT.y);

		// uv coordinates for vertex
		// 1. project vertex position to texel (s,t) coordinates
		// 2. bsp scale is inches, engine is in meters
		// 3. apply texture offset
		// 4. normalize to 0-1 range for opengl sampling
		float u = (glm::dot(position, s) / BSP_TO_ENGINE_UNIT_CONVERSION + texInfo.fSShift) / texture.width;
		float v = (glm::dot(position, t) / BSP_TO_ENGINE_UNIT_CONVERSION + texInfo.fTShift) / texture.height;

		vertex.uv = glm::vec2(u, v);
		vertices.push_back(vertex);
	}


	// fan triangulate the convex polygon
	// vertex 0 is the hub, connect to each subsequent edge
	for (int i = 1; i < faceVertices.size() - 1; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	return new Mesh(vertices, indices);
}

int BSPMap::traceHullSegment(const int nodeIndex, const glm::vec3& segmentStart, const glm::vec3& segmentEnd) {
	
	// epsilon (helps with ramps/inclines)
	const float acceptanceEpsilon = -0.03125f;

	// negative index is a leaf, base case
	if (nodeIndex < 0)
		return nodeIndex;

	const BSPClipNode& cNode = bsp.clipNodes()[nodeIndex];
	const BSPPlane& cPlane = bsp.planes()[cNode.iPlane];

	float segmentStartDistance = glm::dot(
		segmentStart,
		glm::vec3(cPlane.vNormal.x, cPlane.vNormal.z, -cPlane.vNormal.y)
	) - cPlane.fDist * BSP_TO_ENGINE_UNIT_CONVERSION;

	float segmentEndDistance = glm::dot(
		segmentEnd,
		glm::vec3(cPlane.vNormal.x, cPlane.vNormal.z, -cPlane.vNormal.y)
	) - cPlane.fDist * BSP_TO_ENGINE_UNIT_CONVERSION;

	// if both points are on positive side of plane, recurse front (0)
	if (segmentStartDistance >= acceptanceEpsilon && segmentEndDistance >= acceptanceEpsilon)
		return traceHullSegment(cNode.iChildren[0], segmentStart, segmentEnd);
	
	// if both points are on negative side of plane, recurse back (1)
	if (segmentStartDistance < acceptanceEpsilon && segmentEndDistance < acceptanceEpsilon)
		return traceHullSegment(cNode.iChildren[1], segmentStart, segmentEnd);

	// i think i have computed the line plane intersection correctly
	// https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection

	// lerp for midpoint
	float t = segmentStartDistance / (segmentStartDistance - segmentEndDistance);
	
	// i_{a} + i_{ab}*t 
	glm::vec3 intersection = segmentStart + t * (segmentEnd - segmentStart);

	int nearSide = segmentStartDistance >= 0 ? 0 : 1;
	int farSide = segmentStartDistance >= 0 ? 1 : 0;

	// check nearside
	int nearContents = traceHullSegment(cNode.iChildren[nearSide], segmentStart, intersection);
	if ((contentType)nearContents != contentType::CONTENTS_EMPTY)
		return nearContents;

	// check farside
	return traceHullSegment(cNode.iChildren[farSide], intersection, segmentEnd);
}