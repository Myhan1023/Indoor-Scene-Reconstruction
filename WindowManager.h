#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "Camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class WindowManager {
public:

	//the process of creating a window
	WindowManager(int width, int height, const char* title);

	//stop the window
	~WindowManager();

    //Whether to close the window
	bool shouldClose();

	//swap the color buffer and check if there is any events
	void swapAndPoll();

	//query the status of key presses (ESC)
	void processInput(Camera& camera, float deltaTime);

	//pass "window" from private
	GLFWwindow* getWindow() {
		return window;
	}

	//the projection matrix of aspect ratio
	float getAspectRatio() {
		return (float)width / (float)height;
	}

private:

	GLFWwindow* window;
	int width, height;

	//when I use mouse to enlarge or shrink the window, let opengl adjust the scope of its picture at the same time
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};
#endif