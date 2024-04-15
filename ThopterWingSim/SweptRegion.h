#ifndef SWEPT_REGION
#define SWEPT_REGION

#include "Vec2.h"
#include <iostream>

class SweptRegion
{
public:
	SweptRegion() :
		wash(),
		sum_impulse(),
		area(0)
	{}
	
	Vec2 wash;
	Vec2 sum_impulse;
	double area;
	Vec2 getDiskVel(double t, double air_dens);
	void addThrust(Vec2 thrust, double dt);
	Vec2 getAvgThrust(double t);
};

#endif
