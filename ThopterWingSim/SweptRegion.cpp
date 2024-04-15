#include "SweptRegion.h"

Vec2 SweptRegion::getDiskVel(double t, double air_dens)
{
	Vec2 avg_thrust = getAvgThrust(t);
	double avg_thrust_mag = avg_thrust.mag();
	double dir = avg_thrust_mag < 0 ? -1 : 1;
	double wash_mag = 0.5 * dir * sqrt(2 * dir * avg_thrust_mag / (air_dens * area));
	wash = avg_thrust.unit() * -wash_mag;
	return wash;
}
void SweptRegion::addThrust(Vec2 thrust, double dt)
{
	sum_impulse += thrust * dt;
}
Vec2 SweptRegion::getAvgThrust(double t)
{
	if(t <= 0)
		return Vec2(0, 0);
	return sum_impulse / t;
}
