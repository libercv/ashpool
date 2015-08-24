#include "model.hpp"
#include "mesh.hpp"

void Model::Draw(const ShaderProgram& shader) const {
	for (const auto& mesh: meshes)
		mesh.Draw(shader);
}
