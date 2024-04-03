#ifndef WING_ELEMENT
#define WING_ELEMENT

#include <cmath>
#include <iostream>
#include "Vec2.h"
#include "Airfoil.h"
#include "PiConsts.h"

struct WingElement
{
	WingElement() : 
		chord(0),
		span(0),
		lift(0),
		drag(0),
		aoa(0),
		cl(0),
		cd(0),
		mass(0)
	{}
	Airfoil* airfoil;
	
	double chord;
	double span;
	double mass;
	Vec2 vel;
	double angle = 0;
	void calculateForces(Vec2 airflow, double air_dens)
	{
//		std::cout << "elem airflow: " << airflow.x << ' ' << airflow.y << std::endl;
		const double rads = angle * pi_over_180;
		
		Vec2 motion_rel = (vel - airflow).rotate(-rads);
		double speed = motion_rel.mag();
		
		double aoa_rads = -atan2(motion_rel.y, motion_rel.x);
		
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
