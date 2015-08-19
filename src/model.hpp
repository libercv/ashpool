#ifndef MODEL_H
#define MODEL_H

#include <string>    // for string
#include <vector>    // for vector
#include <glm/glm.hpp> 
#include "mesh.hpp"  // for Mesh
class Shader;


class Model
{
	public:
		/*  Functions   */
		// Constructor, expects a filepath to a 3D model.
		explicit Model(const std::string& path);

		// Draws the model, and thus all its meshes
		void Draw(const Shader& shader) const;
		const glm::mat4 *getModelMatrix() { return &modelMatrix; }; 

	private:
		/*  Model Data  */
		std::vector<Mesh> meshes;
		std::string directory;
		glm::mat4 modelMatrix;
		


};

#endif // MODEL_H
