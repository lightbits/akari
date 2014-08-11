#ifndef CAMERA_H
#define CAMERA_H
#include "matrix.h"

class Camera
{
public:
	Camera();

	void rotateHorizontal(float dt);
	void rotateVertical(float dt);

	void moveUp(float dx);
	void moveDown(float dx);
	void moveLeft(float dx);
	void moveRight(float dx);
	void moveForward(float dx);
	void moveBackward(float dx);

	void setHorizontalAngle(float radians);
	void setVerticalAngle(float radians);
	void setPosition(const vec3 &p);

	mat4 getViewMatrix();
	mat4 getViewMatrixFocus(float radius, const vec3 &focus);
private:
	void updateVectors();

	float theta;
	float phi;
	vec3 position;
	vec3 up;
	vec3 right;
	vec3 forward;
};

#endif