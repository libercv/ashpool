#ifndef MODEL_H
#define MODEL_H

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
class TextureManager;


class Model 
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(const std::string& path);

    // Draws the model, and thus all its meshes
    void Draw(const Shader& shader);
    
private:
    /*  Model Data  */
    std::vector<Mesh> meshes;
    std::string directory;

    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string& path);

};

#endif // MODEL_H
