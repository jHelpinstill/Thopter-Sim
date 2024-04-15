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
	
	Airfoil* airfoil;

	double sum_impulse = 0;
	double sum_energy = 0;
	
public:
	Blade(){}
	Blade(double span, double chord_root, double chord_tip, double mass, Airfoil* af, int num_elems, double amplitude);
	
	void update(Vec2 airflow, double air_dens, double dt, bool print_elems = false);
	
	/// SETTINGS
	double span = 0;
	double chord_root = 0;
	double chord_tip = 0;
	double mass = 0;	
	double amplitude = 0;
	double collective = 0;
	double freq = 0;
	double sweep_plane_angle = 0;

	/// OUTPUTS
	double t = 0;
	double angular_position = 0;
	double axial_thrust = 0;
	double transverse_thrust = 0;
	Vec2 force;
	double torque = 0;
	double transverse_moment = 0;
	double lift_moment = 0;
	double peak_torque = 0;
	double peak_transverse_moment = 0;
	double peak_lift_moment = 0;
	double power = 0;
	double specific_thrust = 0;
	
	void printElems();
	void printRegions();
	void printDebug();
	void printInfo(bool verbose = false);
		
	void reset();
	void build();
	void rebuild();
	
	~Blade();
};


#endif
