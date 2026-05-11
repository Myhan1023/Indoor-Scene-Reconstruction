#include "stb_image.h"
#include "Texture.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>


unsigned int loadTexture(const char* path) {

	//apply ID
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//loading to ram

	//the image's width, height, and Number of channels(RGB or RGBA)

	int width, height, nrComponents;

	//image flipping

	stbi_set_flip_vertically_on_load(true);

	//fiva Parameters( image path, image width, image height, image Number of channels, Output the number of channels truthfully)

	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

	//Determine the data format



	if (data) {

		GLenum format;

		if (nrComponents == 1) format = GL_RED;
		else if (nrComponents == 3) format = GL_RGB;
		else if (nrComponents == 4) format = GL_RGBA;

		//pass data to ram

		glBindTexture(GL_TEXTURE_2D, textureID);

		//make the data of image in cpu to gpu

		//nine Parameters( texture type, mipmap level, internal format, image width, image height, border, data format, data type, image data)

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		//Automatically enlarge or shrink the picture.

		glGenerateMipmap(GL_TEXTURE_2D);

		//texture wrapping

		//three parameters( texture type, s -> X-axis / t -> Y-axis, wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Texture Filtering

		//three parameters( texture type, Attribute category, Specific plan)

		//minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR_MIPMAP_LINEAR);

		//magnification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//release the image data in cpu

		stbi_image_free(data);

	}
	else {

		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}