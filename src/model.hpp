#ifndef MODEL_H
#define MODEL_H

#include <string>    // for string
#include <vector>    // for vector
#include <glm/glm.hpp> 
#include "mesh.hpp"  // for Mesh
class ShaderProgram;


class Model
{
	public:
		/*  Functions   */
		// Constructor, expects a filepath to a 3D model.
		Model() = default;
		Model(std::vector<Mesh> m) : meshes{std::move(m)} {};
		Model& operator=(const Model& other) = delete;
		Model(const Model &other) = delete;
	
		Model(Model &&other) : meshes{std::move(other.meshes)},   
				//directory{std::move(other.directory)} ,
				modelMatrix{std::move(other.modelMatrix) } {
			std::cout << "Model: move ctor" << std::endl; };


		// Draws the model, and thus all its meshes
		void Draw(const ShaderProgram& shader) const;
		const glm::mat4 *getModelMatrix() { return &modelMatrix; }; 

	private:
		/*  Model Data  */
		std::vector<Mesh> meshes;
		//std::string directory;
		glm::mat4 modelMatrix {};
};

#endif // MODEL_H
