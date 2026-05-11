#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <string>
#include <glad/glad.h> //save unsigned int
#include <sstream>
#include <iostream>
#include <unordered_map> //hash table
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>




class Shader {
public:
	//define ID
	unsigned int ID;

	//function
	Shader(const char* vertPath, const char* fragPath);

	//activate 
	void use() const;

	//uniform's bag, enable to reduce times
	//feature switch
	void setBool(const std::string& name, bool value)const;

	//bind texture
	void setInt(const std::string& name, int value)const;

	//pass a single continuously changing value
	void setFloat(const std::string& name, float value)const;

	//pass color (x, y, z)
	void setVec3(const std::string& name, float x, float y, float z)const;

	//pass 4*4 matrix
	void setMat4(const std::string& name, const glm::mat4& mat) const;

private:

	////Review:to check the errrors
	void checkCompileErrors(unsigned int shader, std::string type);

	std::string readFile(const char* filePath);
};
#endif // !SHADER_H