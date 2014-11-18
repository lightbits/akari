#include "laser.h"

void update_laser(Laser &l, float dt)
{
	if (l.cooldown > 0.0f)
		l.cooldown -= dt;
}