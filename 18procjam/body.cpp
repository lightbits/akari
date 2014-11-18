#include "body.h"

void update_body(Body &body, float dt)
{
	body.position += body.velocity * dt;
	body.velocity += body.acceleration * dt;
}

void clamp_speed(float max_speed, Body &body)
{
	float speed = length(body.velocity);
	if (speed > max_speed)
		body.velocity = normalize(body.velocity) * max_speed;
}