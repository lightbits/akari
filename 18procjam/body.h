#ifndef BODY_H
#define BODY_H
#include "kyoko.h"

struct Body
{
	vec2 size;
	vec2 position;
	vec2 velocity;
	vec2 acceleration;
};

void update_body(Body &body, float dt);
void clamp_speed(float max_speed, Body &body);

#endif