#ifndef MESH_H
#define MESH_H

#include "Texture.h"

#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Transform;
class Program;

class Mesh {
public:
	Mesh();
	Mesh(std::vector<Texture>&& textures,
		std::vector<glm::vec3>&& vertices,
		std::vector<glm::vec2>&& uvs,
		std::vector<glm::vec3>&& normals,
		std::vector<unsigned short>&& indices
	);

	Mesh(Mesh&& rhs) noexcept;

	~Mesh();

	void create_buffers();

	void draw(const Program* program, int mode, const Transform transform) const;

	std::vector<Texture>				_textures;
	std::vector<glm::vec3>				_vertices;
	std::vector<glm::vec2>				_uvs;
	std::vector<glm::vec3>				_normals;
	std::vector<unsigned short>			_indices;
private:
	GLuint								_vao, _vertex_buffer, _uv_buffer, _normal_buffer, _indices_buffer;
};

#endif