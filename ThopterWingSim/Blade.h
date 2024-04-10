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
	
	int ID;
	static int next_ID;
	
public:
	Blade() : ID(next_ID++){}
	Blade(double span, double chord_root, double chord_tip, double mass, Airfoil* af, int num_elems, double amplitude);
	
	double t = 0;
	
	void update(Vec2 airflow, double air_dens, double dt, bool print_elems = false);
	
	double span = 0;
	double chord_root = 0;
	double chord_tip = 0;
	double mass = 0;
	
	double amplitude = 0;
	double collective = 0;
	double freq = 0;
	double sweep_plane_angle = 0;
	
	double thrust = 0;
	double torque = 0;
	double torque_AC = 0;
	
	double peak_torque = 0;
	double peak_torque_AC = 0;
	double peak_lift_moment = 0;
	double specific_power = 0;
	
	double getAvgThrust();
	double getAvgPower();
	
	void printElems();
	void printRegions();
	void printDebug();
	void printInfo(bool verbose = false);
	
	void setSpan(double span);
	void setChordRoot(double chord_root);
	void setChordTip(double chord_tip);
	void setMass(double mass);
	void setAmplitude(double amplitude);
	void setCollective(double collective);
	void setFreq(double freq);
	void setSweepPlaneAngle(double sweep_plane_angle);
		
	void reset();
	void build();
	void rebuild();
	
	~Blade();
};


#endif
