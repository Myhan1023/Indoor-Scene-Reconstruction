#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glad/glad.h>

// Constructor implementation

Camera::Camera(glm::vec3 position,
	glm::vec3 up,
	float yaw,
	float pitch,
	float zoom)
	//Member initializer list

	//front, look at screen(z-axis)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch), Zoom(zoom)
{

	//calculate the initial vecotr（local coordinate system:x,y,z)

	UpdateCameraVectors();

}

//Get Matrix

glm::mat4 Camera::GetViewMatrix() {

	//three Parameter (eye's position, eye's look at, head's position)
	return glm::lookAt(Position, Position + Front, Up);
}

//Movement Keyboard

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {

	//the distance moved per frame, velocity (distance), movementSpeed (speed), delteTime (time)
	//Ensure the speed remains consistent under different frame rates

	float velocity = MovementSpeed * deltaTime;

	if (direction == FORWARD) {
		Position += Front * velocity;
	}
	if (direction == BACKWARD) {
		Position -= Front * velocity;
	}
	if (direction == RIGHT) {
		Position += Right * velocity;
	}
	if (direction == LEFT) {
		Position -= Right * velocity;
	}

}

//rotate mouse

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch ) {

	//Shrink the pixels of the mouse movement through the sensitivity setting and then convert them into the 3D world.

	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	//let this per's offset in total angel

	Yaw += xoffset;
	Pitch += yoffset;

	//limit the pitch angel
	if (constrainPitch) {
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	//use new angel to calculate vectors

	UpdateCameraVectors();
}

//caculate

void Camera::UpdateCameraVectors() {

	//calculate Front

	glm::vec3 front;

	//glm::radians(degree) = degree * ( PI / 180.0f )

	front.x = glm::cos(glm::radians(Pitch)) * glm::cos(glm::radians(Yaw));
	front.y = glm::sin(glm::radians(Pitch));
	front.z = glm::cos(glm::radians(Pitch)) * glm::sin(glm::radians(Yaw));

	//Normalize to ensure the step size is 1

	Front = glm::normalize(front);

	//calculate Right

	Right = glm::normalize(glm::cross(Front, WorldUp));

	//calculate Up

	Up = glm::normalize(glm::cross(Right, Front));

}

//receive displacement of the mouse whell and change zoom value
//scroll offset

void Camera::ProcessMouseScroll(float yoffset) {

	//change view value, make object bigger, reduce zoom(fov view) 
	Zoom -= yoffset;

	//constraint range

	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;

}

//look at the center of the object

void Camera::LookAt(glm::vec3 target) {
	
	//A vector pointing from the current position to the target

	glm::vec3 direction = glm::normalize(target - Position);

	//calculate pitch

	Pitch = glm::degrees(asin(direction.y));

	//calculate yaw

	Yaw = glm::degrees(atan2(direction.z, direction.x));

	UpdateCameraVectors();
}