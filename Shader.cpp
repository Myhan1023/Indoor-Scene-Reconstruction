#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

//make .vert and .frag file in string

std::string Shader::readFile(const char* filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	if (!fileStream.is_open()) {
		std::cerr << "Could not read file" << filePath << ". File doesn't exist" << std::endl;
		return "";
	}
	std::stringstream sstr;
	sstr << fileStream.rdbuf();
	content = sstr.str();
	fileStream.close();
	return content;
}

//Lighting model

//compilation and linking of shader

Shader::Shader(const char* vertPath, const char* fragPath) {

	//use morden OpenGL to read shader source code from .vert and .frag file
	std::string vertCode = readFile(vertPath);
	//read the first line of the .vert file to get the shader type
	const char* vShaderCode = vertCode.c_str();

	std::string fragCode = readFile(fragPath);
	const char* fShaderCode = fragCode.c_str();

	//Compilation of vertex shader

	//apply ID for vertex shader
	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);

	//four Parameter (shader ID, need the number of string, Code address, length of string)
	glShaderSource(vertex, 1, &vShaderCode, NULL);

	//start to compile the vertex shader
	glCompileShader(vertex);

	//Review
	checkCompileErrors(vertex, "VERTEX");

	//Compilation of fragment shader

	//apply ID for fragment shader
	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragment, 1, &fShaderCode, NULL);

	//start to compile the fragment shader
	glCompileShader(fragment);

	//Review
	checkCompileErrors(fragment, "FRAGMENT");

	//Shader Program

	this->ID = glCreateProgram();

	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);

	glLinkProgram(ID);

	//Review
	checkCompileErrors(ID, "PROGRAM");

	//clean
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

//release " use " function

void Shader::use() const {
	glUseProgram(ID);
}

//feature switch
void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

//bind texture

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

//pass a single continuously changing value

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

//pass color (x, y, z)

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

//pass 4*4 matrix

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {

	//four parameters (ID, the number of the matrix, whether to transpose, the original pointer of the matrix(mat))
	
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));

}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {

	int success;
	char infolog[1024];

	if (type != "PROGRAM") {

		//ask compilation status of vertex/fragment shader, if failed, print the error message
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {

			//four Parament (shader ID, max length of error message, length of this error message, the writing place of error message)  
			glGetShaderInfoLog(shader, 1024, NULL, infolog);

			std::cerr << "SHADER_COMPILATION_ERROR of type: " << type << "\n" << infolog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);

		if (!success) {

			glGetProgramInfoLog(shader, 1024, NULL, infolog);
			std::cerr << "PROGRAM_LINKING_ERROR of type: " << type << "\n" << infolog << std::endl;
		}
	}
}