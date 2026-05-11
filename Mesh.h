#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

//Data reconstruction

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;

	//add new coordinate "uv" [0,1]
	glm::vec2 TexCoords;

};

//texture 

struct Texture {
	
	unsigned int id;

	//the name like "wallTexture" in Shader

	std::string type;
};

class Mesh {
public:

	//apply ID for vao and vbo and ebo
	unsigned int VAO;

	//add new tectures

	std::vector<Texture> textures;

	//pass vertices and indices to build the model in the VRAM
	Mesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const std::vector<Texture> textures);
	
	//stop the draw
	~Mesh();

	//the only input of the render 
	void Draw(Shader& shader);

private:

	unsigned int VBO, EBO;
	unsigned int indicesCount;

	void setupMesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices);
};
#endif // !MESH_H

