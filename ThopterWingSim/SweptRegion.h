#ifndef SWEPT_REGION
#define SWEPT_REGION

#include "Vec2.h"
#include <iostream>

class SweptRegion
{
public:
	SweptRegion() :
		wash(0),
		sum_impulse(0),
		area(0)
	{}
	
	double wash;
	double sum_impulse;
	double area;
	Vec2 getDiskVel(double t, double air_dens);
	void addThrust(double thrust, double dt);
	double getAvgThrust(double t);
};

#endif
