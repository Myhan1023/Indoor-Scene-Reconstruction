#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

//Define several position names for camera movement, such as the number of 0 is FORWARD.
enum Camera_Movement {
	//0
	FORWARD,
	//1
	BACKWARD,
	//2
	LEFT,
	//3
	RIGHT
};

//use Class to encapsulate the camera information and operations

class Camera {
public:
	//Public API

    // --- 1. Camera Attributes ---
	//the camera's position in the world space
	//spatial coordinate system

	//my position is the start position

	glm::vec3 Position;

	//local coordinate axes

	//view vector
	glm::vec3 Front;

	glm::vec3 Up;
	glm::vec3 Right;

	//define the up vector, which is the y-axis in the world space

	glm::vec3 WorldUp; //anchor point
	
    // --- 2. Euler Angles ---
	//control angle information 
	
	//initial look direction is -Z_axis

	float Yaw;

	//view, high or low or horizontal view

	float Pitch;

    // --- 3. Camera Options ---
    //stutas information
	
	//distance moved in one second 
	
	float MovementSpeed;

	//your head rotate in the 3D world when the mouse moves 1 pixel on the desktop
	
	float MouseSensitivity;

    // --- 4. lens Properties
    
	float Zoom;

    // --- 5. init the camera's position, up vector and front vector

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = -90.0f,
		float pitch = 0.0f,
		float zoom = 45.0f);

    // --- 6. get View Matrix

	glm::mat4 GetViewMatrix();

    // --- 7. control keyboard 

	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // --- 8. control mouse

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

	// --- 9. scroll offset

	void ProcessMouseScroll(float yoffset);

	// --- 10. look at the object

	void LookAt(glm::vec3 target);

private:

	void UpdateCameraVectors();
};