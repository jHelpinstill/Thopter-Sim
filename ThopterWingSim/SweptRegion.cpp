#include "SweptRegion.h"

Vec2 SweptRegion::getDiskVel(double t, double air_dens)
{
	double dir = getAvgThrust(t) < 0 ? -1 : 1;
	wash = 0.5 * dir * sqrt(2 * dir * getAvgThrust(t) / (air_dens * area));
	return Vec2(0, -wash);
}
void SweptRegion::addThrust(double thrust, double dt)
{
	sum_impulse += thrust * dt;
}
double SweptRegion::getAvgThrust(double t)
{
	if(t <= 0)
		return 0;
	return sum_impulse / t;
}
