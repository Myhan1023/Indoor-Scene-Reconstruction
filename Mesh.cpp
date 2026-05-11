#include "Mesh.h"
#include <glad/glad.h>

//pass vertices and indices to build the model in the VRAM

Mesh::Mesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const std::vector<Texture> textures) {
	this->indicesCount = static_cast<unsigned int>(indices.size());
	this->textures = textures;
	setupMesh(vertices, indices);
}

//stop the draw
Mesh::~Mesh() {
	//free memory and end program

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

//the only input of the render 
void Mesh::Draw(Shader& shader) {

	//make textures in gpu before drawing

	for (unsigned int i = 0; i < textures.size(); i++) {

		//active texture (GL_TEXTURE0,1...)

		glActiveTexture(GL_TEXTURE0 + i);

		//tell shader: which texture is the right sampler 
		
		shader.setInt(textures[i].type, i);

		//make real pixtures in texture[i]

		glBindTexture(GL_TEXTURE_2D, textures[i].id);

	}

	//bind vao to draw

	glBindVertexArray(VAO);

	//four Parameter (rendering model, number of indices, type of indices, where to start)
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCount), GL_UNSIGNED_INT, 0);

	//Clear the current VAO binding to 0.

	glBindVertexArray(0);

	//After drawing, restore the texture unit to the default state

	glActiveTexture(GL_TEXTURE);

}

//data from ram to vram, Explanation: Problem (Vertex attribute pointer), (Encapsulation of VAO)
void Mesh::setupMesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices) {

	//apply ID for vao and vbo and ebo
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);

	glGenBuffers(1, &EBO);

	//active instruction of vao

	glBindVertexArray(VAO);

	//select current buffer to operate

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//teleport the data of vbo to the buffer of OpenGL 

	//four Parameter (target'buffer tpye' , size of data, data address, usage)
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	//Core

	//teleport the data of ebo to the buffer of OpenGL

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//Location attribute (location = 0)
	//six Parameter (index of vertex_shader.vert's layout, size, type, normalized, stride, offset)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//active the slot 0

	glEnableVertexAttribArray(0);

	//Normal attribute (location = 1)
	//the last parameter imeans: jump the first three numbers

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

	//active the slot 1

	glEnableVertexAttribArray(1);

	//Location attribute (location = 2)
	//the last parameter imeans: jump the first three numbers(location coordinate) and three numbers(normal vector)
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
	
	//active the slot 2

	glEnableVertexAttribArray(2);

	//Clear the current data binding to 0.

	glBindVertexArray(0);
}