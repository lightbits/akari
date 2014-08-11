#include "camera.h"
#include "types.h"

Camera::Camera() : theta(-PI), phi(0), position(0, 0, -1)
{
	updateVectors();
}

void Camera::rotateHorizontal(float dt) 
{
	theta = glm::mod(theta + dt, TWO_PI);
}

void Camera::rotateVertical(float dt) 
{ 
	phi = glm::clamp(phi - dt, -PI_TWO, PI_TWO);
}

void Camera::moveUp(float dx)
{ position += up * dx; }

void Camera::moveDown(float dx)
{ position -= up * dx; }

void Camera::moveLeft(float dx)
{ position -= right * dx; }

void Camera::moveRight(float dx)
{ position += right * dx; }

void Camera::moveForward(float dx)
{ position += forward * dx; }

void Camera::moveBackward(float dx)
{ position -= forward * dx; }

void Camera::setHorizontalAngle(float t) 
{ 
	theta = glm::mod(t, TWO_PI); 
}

void Camera::setVerticalAngle(float t) 
{ 
	phi = glm::mod(abs(t), PI) * (t < 0 ? -1 : 1); 
}

void Camera::setPosition(const glm::vec3 &p) 
{ 
	position = p;
}

void Camera::updateVectors()
{
	float sintheta = sinf(theta);
	float costheta = cosf(theta);
	float sinphi = sinf(phi);
	float cosphi = cosf(phi);
	forward = vec3(cosphi * sintheta, sinphi, -cosphi * costheta);
	right = vec3(costheta, 0.0f, sintheta);
	up = glm::cross(right, forward);
}

mat4 Camera::getViewMatrix() 
{ 
	updateVectors();
	return glm::lookAt(position, position + forward, up); 
}

mat4 Camera::getViewMatrixFocus(float radius, const vec3 &focus) 
{
	updateVectors();
	return glm::lookAt(focus + radius * forward, focus, up); 
}