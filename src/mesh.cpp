#include "mesh.h"
#include "shader.h"
#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texturemanager.h"

Mesh::Mesh(std::vector<Vertex> Vertices,  std::vector<GLuint> Indices, std::vector<Texture> Textures) :
    	vertices {std::move(Vertices)}, indices {std::move(Indices)}, textures {std::move(Textures)} {
		// Now that we have all the required data, set the vertex buffers and its attribute pointers.
	this->setupMesh();
}

Mesh::Mesh(const aiMesh* mesh, const aiScene* scene, const std::string &directory) {
	vertices.reserve(mesh->mNumVertices);
	for(GLuint i = 0; i < mesh->mNumVertices; i++) {
		auto vv = &mesh->mVertices[i];
		auto vn = &mesh->mNormals[i];
		// Texture Coordinates
		if(mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?  
			auto vt = &mesh->mTextureCoords[0][i];
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vertices.emplace_back( glm::vec3(vv->x, vv->y, vv->z),
				glm::vec3(vn->x, vn->y, vn->z),
				glm::vec2(vt->x, vt->y)  );
		} else {
			vertices.emplace_back( glm::vec3(vv->x, vv->y, vv->z),
				glm::vec3(vn->x, vn->y, vn->z),
				glm::vec2(0.0f, 0.0f)  );
		}
	}
	// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	indices.reserve(3 * mesh->mNumFaces);
	for(GLuint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		std::copy_n(&face.mIndices[0], face.mNumIndices, back_inserter(indices) );
		//indices.insert(indices.end(), &face.mIndices[0], &face.mIndices[face.mNumIndices]);
	}

	// Process materials
	if(mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		std::vector<Texture> diffuseMaps = TextureManager::get().loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		std::vector<Texture> specularMaps = TextureManager::get().loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	
		this->mat = loadMaterial(material);	
		//glGenBuffers(1,&uniformBlockIndex);
		//glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockIndex);
		//glBufferData(GL_UNIFORM_BUFFER, sizeof(mat), (void *)(&mat), GL_STATIC_DRAW);
	}
	this->setupMesh();
}

struct Material Mesh::loadMaterial(aiMaterial *mtl) {
		aiString texPath;	//contains filename of texture
		struct Material aMat;

		if(mtl->GetTextureCount(aiTextureType_DIFFUSE)>0)
			aMat.texCount = 1;
		else
			aMat.texCount = 0;

		glm::vec4 vec(0.8f, 0.8f, 0.8f, 1.0f);
		aiColor4D diffuse;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
			vec.x = diffuse.r; vec.y=diffuse.g; vec.z=diffuse.b; vec.w=diffuse.a;
		}
		aMat.diffuse = vec;

		vec = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		aiColor4D ambient;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
			vec.x = ambient.r; vec.y=ambient.g; vec.z=ambient.b; vec.w=ambient.a;
		}
		aMat.ambient=vec;

		aiColor4D specular;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular)) 
			aMat.specular=glm::vec4(specular.r, specular.g, specular.b, specular.a);
		else
			aMat.specular=glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


		aiColor4D emission;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
			aMat.emissive=glm::vec4(emission.r, emission.g, emission.b, emission.a);
		else
			aMat.emissive=glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


		float shininess = 0.0;
		unsigned int max;
		aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
		aMat.shininess = shininess;
	
		return aMat;
}
	


// Render the mesh
void  Mesh::Draw(const Shader& shader) {
	// Bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for(GLuint i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		// Retrieve texture number (the N in diffuse_textureN)
		std::stringstream ss;
		std::string number;
		std::string name = this->textures[i].type;
		if(name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if(name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		number = ss.str(); 
		// Now set the sampler to the correct texture unit
		glUniform1i(shader.getUniformLocation(name + number), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}

	// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
	glUniform1f(shader.getUniformLocation("material_shininess"), mat.shininess);
	glUniform4f(shader.getUniformLocation("material_diffuse"), mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, mat.diffuse.w);
	glUniform4f(shader.getUniformLocation("material_ambient"), mat.ambient.x, mat.ambient.y, mat.ambient.z, mat.ambient.w);
	glUniform1i(shader.getUniformLocation("material_texCount"), mat.texCount);
        //glBindBufferRange(GL_UNIFORM_BUFFER, materialUniLoc, myMeshes[nd->mMeshes[n]].uniformBlockIndex, 0, sizeof(struct MyMaterial));	

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::setupMesh() {
	// Create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	auto sizeof_vertex = sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof_vertex, &this->vertices[0], GL_STATIC_DRAW);  

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof_vertex, (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof_vertex, (GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof_vertex, (GLvoid*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}


Mesh::~Mesh() {
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}
	


