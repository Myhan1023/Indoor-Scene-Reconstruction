#include "WindowManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

WindowManager::WindowManager(int width, int height, const char* title) 
    : width(width),height(height),window(nullptr) {

	//init glfw

	glfwInit();

	//use opengl 3.3 core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create window

	window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL) {

		std::cout << "Failed to create GLFW window" << std::endl;

		//free memory and end program

		glfwTerminate();

		//means error
		return;
	}

	glfwMakeContextCurrent(window);

	// init glad

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		
		std::cout << "Failed to initiakize GLAD" << std::endl;
		
		return;
	}

	//Z-Buffer

	glEnable(GL_DEPTH_TEST);
	
	//capture and hide the mouse

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Register our sentinel for the adaptive viewport

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
};


WindowManager::~WindowManager() {
	if (window) {
		glfwTerminate();
	}
}

//whether to close

bool WindowManager::shouldClose() {
	return glfwWindowShouldClose(window);
}

//swap the color buffer and check if there is any events

void WindowManager::swapAndPoll() {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void WindowManager::processInput(Camera& camera, float deltaTime) {
	//exit the program

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	//Polling: every time when " w/a/s/d " is pressed

	//print camera's coordinate

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		std::cout << "Camera Pos: x=" << camera.Position.x
			<< "Camera Pos: y=" << camera.Position.y
			<< "Camera Pos: z=" << camera.Position.z << std::endl;
	}

	//keyboard movement

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

void WindowManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}