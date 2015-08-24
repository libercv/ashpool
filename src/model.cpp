#include "model.hpp"
#include <vector>                // for vector
#include "mesh.hpp"              // for Mesh

// Draws the model, and thus all its meshes
void Model::Draw(const ShaderProgram& shader) const {
	for (const auto& mesh: meshes)
		mesh.Draw(shader);
}
