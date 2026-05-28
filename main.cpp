#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "WindowManager.h"
#include "SemanticAnalyzer.h"
#include "PipeLine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <opencv2/opencv.hpp>

void runPhase3_PixelToPhysical(std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int runPhase2_PixelToPhysical();
// Global Camera

//Camera's position,(z = 5.0f, out of screen)
Camera camera(glm::vec3(1.0f, 1.0f, 0.5f));

//by default, the mouse starts in the middle
float lastX = 500.0f;
float lastY = 400.0f;

//the first meeting
bool firstMouse = true;

//achieve frame rate indepence

//what happened to the last frame
float lastFrame = 0.0f;

//current frame - last frame
float deltaTime = 0.0f;

int main() {

	//window init

	WindowManager app(1000, 800, "Forest");

	//check the window whether to make
	
	if (app.getWindow() == NULL) {
		return -1;
	}

	//all window need from app
	//call back the mouse, don't put the function (call_backmouse) in the render(while)

	glfwSetCursorPosCallback(app.getWindow(), mouse_callback);

	//call back the scroll, don't put the function (call_backscroll) in the render(while)

	glfwSetScrollCallback(app.getWindow(), scroll_callback);

	//use Class Shader

	Shader myshader("Vertex_shader.vert", "Fragment_shader.frag");
    
	//Texture loading
	
	unsigned int wallID = loadTexture("assets/wall.jpg");
	unsigned int floorID = loadTexture("assets/floor.jpg");
	unsigned int doorID = loadTexture("assets/door.jpg");
	//make in struct
	
	Texture wallTexture ={
		wallID,
		"wallTexture"
	};

	Texture floorTexture = {
		floorID,
		"floorTexture"
	};

	Texture doorTexture = {
		doorID,
		"doorTexture"
	};

	//create vertices and indices

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> houseTextures;

	houseTextures.push_back(wallTexture);
	houseTextures.push_back(floorTexture);
	houseTextures.push_back(doorTexture);

	runPhase2_PixelToPhysical();
	//run phase3 to fill vertices and indices
	runPhase3_PixelToPhysical(vertices, indices);

	std::cout << "Vertices size: " << vertices.size() << std::endl;
	std::cout << "Indices size: " << indices.size() << std::endl;

	//the process of VAO, VBO, EBO
	
	Mesh myMesh(vertices, indices, houseTextures);

	//before, define the camera's working position
	
	camera.Position = glm::vec3(0.0f, 2.5f, 10.0f);

	//look at the camera
	
	camera.LookAt(glm::vec3(-2.0f, -2.0f, 0.0f));

	//Render
	
	while (!glfwWindowShouldClose(app.getWindow())) {

		//Time
		
		//get the total time of the program(s)
		
		float currentFrame = static_cast<float>(glfwGetTime());

		//get the time of this frame
		
		deltaTime = currentFrame - lastFrame;

		//update the memory
		
		lastFrame = currentFrame;

		//query the status of key presses
		
		app.processInput(camera, deltaTime); //esc / w / s / a / d

		//Define the background color

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		//clean color and depth buffer 

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//start to use shader program
        
		myshader.use();

		//make the light move back and forth on the X-axis (-5.0, 5.0)

		float lightX = sin(currentFrame) * 5.0f;

		//make the light move back and forth on the Z-axis (-5.0, 5.0)
		
		float lightZ = cos(currentFrame) * 5.0f;

		//light Position

		myshader.setVec3("lightPos", lightX, 8.0f, lightZ);

		//view/camera Position

		myshader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

		//light color
		
		myshader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		//Scale

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		//Rotation
		//three Parameter (the original matrix, rotation time, rotation axis)
		//glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, 0.0f));
		
		//View
		//two Parameter (the original matrix, camera position(usually,opengl define (0,0,0) ) )
		
		glm::mat4 view = camera.GetViewMatrix();
		
		//Projection
		//four Parameter (field of view, aspect ratio, near plane, far plane)
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), app.getAspectRatio(), 0.1f, 100.0f);
		
		//mix (projection, view, model, scale) to get the final transformation matrix (mvp)
		
		glm::mat4 mvp = projection * view * model * scale;

        //get a ID for the uniform variable "mv4" in vertex shader
		myshader.setMat4("mvp", mvp);

		//use Mesh interface to draw
		
		myMesh.Draw(myshader);

		//swap the color buffer and check if there is any events

		app.swapAndPoll();
	}

	return 0;
}

//looking: where to look (calculate offset)

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	//initial startup protection (drop the first frame)

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//calculate offset

	float xoffset = xpos - lastX;

	//y-axis: reverse and increas downword (0 -> 1080)

	float yoffset = lastY - ypos; 

	//upate the memory

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//scroll call back

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
