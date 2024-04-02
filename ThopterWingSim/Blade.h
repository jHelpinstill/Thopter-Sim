#ifndef BLADE_
#define BLADE_

#include "WingElement.h"
#include "SweptRegion.h"

class Blade
{
private:	
	WingElement* elems;
	SweptRegion** regions;
	int num_elems = 0;
	int regions_per_elem = 8;
	
	double span = 0;
	double chord = 0;

	double sum_impulse = 0;
	double sum_energy_squared = 0;
	
public:
	Blade(){}
	Blade(double span, double chord, Airfoil* af, int num_elems, double amplitude);
	
	double t = 0;
	
	// swing attributes (deg, deg, Hz);
	double amplitude = 0;
	double collective = 0;
	double freq = 0;
	
	void update(Vec2 airflow, double air_dens, double dt, bool print_elems = false);
	
	double thrust = 0;
	double torque = 0;
	
	double getAvgThrust();
	double getRMSPower();
	double getAvgPropWash();
	
	void printElems();
	void printRegions();
	
	~Blade();
};


#endif
