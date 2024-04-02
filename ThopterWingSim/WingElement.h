#ifndef WING_ELEMENT
#define WING_ELEMENT

#include <cmath>
#include "Vec2.h"
#include "Airfoil.h"
#include "PiConsts.h"

struct WingElement
{
	WingElement(){}
	Airfoil* airfoil;
	
	double chord;
	double span;
	Vec2 vel = Vec2(0, 0);
	double angle = 0;
	void calculateForces(Vec2 airflow, double air_dens)
	{
		const double rads = angle * pi_over_180;
		
//		std::cout << "vel.x: " << vel.x << " vel.y: " << vel.y;
		
		Vec2 motion_rel = (vel - airflow).rotate(-rads);
		
//		std::cout << "a.x: " << airflow.x << " a.y: " << airflow.y;
		double speed = motion_rel.mag();
		
		double aoa_rads = -atan2(motion_rel.y, motion_rel.x);
		
//		std::cout << " aoa: " << aoa_rads << std::endl;
		aoa = aoa_rads / pi_over_180;
		cl = airfoil->getCl(aoa);
		cd = airfoil->getCd(aoa);
		
		double q = 0.5 * air_dens * chord * span * speed * speed;
		
		lift = q * cl;
		drag = q * cd;
		force = Vec2(-drag, lift).rotate(-aoa_rads).rotate(rads);
		
	}
	
	//output stuff
	double lift;	// N
	double drag;
	double aoa;
	double cl;
	double cd;
	Vec2 force;
};

#endif
