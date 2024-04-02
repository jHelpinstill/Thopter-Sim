#ifndef BLADE_
#define BLADE_

#include "WingElement.h"

class Blade
{
private:
	WingElement* elems;
	int num_elems;
	
	double span;
	double chord;

	double sum_impulse = 0;
	double sum_energy_squared = 0;
	
public:
	Blade(){}
	Blade(double span, double chord, Airfoil* af, int num_elems);
	
	double t = 0;
	
	// swing attributes (deg, deg, Hz);
	double amplitude;
	double collective;
	double freq;
	
	void update(Vec2 airflow, double air_dens, double dt);
	
	double thrust;
	double torque;
	
	double getAvgThrust();
	double getRMSPower();
	
	void printElems();
	
	~Blade();
};


#endif
