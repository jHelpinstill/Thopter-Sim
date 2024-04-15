#include "SweptRegion.h"

Vec2 SweptRegion::getDiskVel(Vec2 v0, double t, double air_dens)
{
	Vec2 F = getAvgThrust(t);
	Vec2 v1 = (v0 + F).unit() * sqrt(v0.sqmag() + 2 * F.mag() / (air_dens * area));
	wash = (v1 + v0) / 2;
	return wash;
//	Vec2 avg_thrust = getAvgThrust(t);
//	double avg_thrust_mag = avg_thrust.mag();
//	double dir = avg_thrust_mag < 0 ? -1 : 1;
//	double wash_mag = 0.5 * dir * sqrt(2 * dir * avg_thrust_mag / (air_dens * area));
//	wash = avg_thrust.unit() * -wash_mag;
//	return wash;
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
