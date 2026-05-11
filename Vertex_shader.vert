//use openGL 3.3
#version 330 core

//data from cpu 
//data tpye:vec3,three float  vector (x,y,z)
//Insert into Slot 0

layout(location = 0) in vec3 aPos; 

//Insert into Slot 1

layout(location = 1) in vec3 aNormal;

//get uv
//Insert into Slot 2

layout(location = 2)in vec2 aTexCoords; 

//MVP, coordinate scaling and view transformation
uniform mat4 mvp;
 
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main() {
    //this process, transform the original coordinates by multiplying with a matrix to convert from "house dimensions" to "screen dimensions" 
	gl_Position = mvp * vec4(aPos, 1.0);

	//pass the object's coordinate to fragment_shder
	//Calculate the distance between the light ray and the object surface
	FragPos = aPos;

	//pass the normal vector to fragment_shder
	//Calculate the reflection angle
	Normal = aNormal;

	//pass uv to fragment
	//the process of calculating pixel coordinate
	TexCoords = aTexCoords;
}