#include <iostream>
#include <cmath>
#include "Airfoil.h"
#include "Vec2.h"

const double pi = 3.14159265;
const double pi_over_180 = pi / 180;
const double two_pi = pi * 2;

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

class Blade
{
private:
	WingElement* elems;
	int num_elems;
	
	double span;
	double chord;

	double sum_impulse = 0;
	double sum_angular_impulse = 0;
	
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
	double getRMSTorque();
	
	~Blade();
};

Blade::Blade(double span, double chord, Airfoil* af, int num_elems) : span(span), chord(chord), num_elems(num_elems)
{
	elems = new WingElement[num_elems];
	for(int i = 0; i < num_elems; i++)
	{
		elems[i].airfoil = af;
		elems[i].chord = chord;
		elems[i].span = span / num_elems;
	}
}

void Blade::update(Vec2 airflow, double air_dens, double dt)
{
	airflow = airflow + Vec2(0, sqrt(2 * getAvgThrust() / (air_dens * (pi * span * span * amplitude / 360)))); // steal thrust momentum from airflow
	
	thrust = 0;
	torque = 0;
	t += dt;
	
	double w = freq * two_pi;
	double twist = collective * cos(w * t);	// degrees
	double angular_vel = -amplitude * pi_over_180 * w * cos(w * t);	// rads
	for(int i = 0; i < num_elems; i++)
	{
		double radius = span * (1 - (double)i / num_elems);
		elems[i].angle = twist;
		elems[i].vel = Vec2(angular_vel * radius, 0);
		elems[i].calculateForces(airflow, air_dens);
		
		thrust += elems[i].force.y;
		torque += elems[i].force.x * radius;
	}
	
	sum_impulse += thrust * dt;
	sum_angular_impulse += torque * torque * dt;
}

double Blade::getAvgThrust()
{
	return sum_impulse / t;
}

double Blade::getRMSTorque()
{
	return sqrt(sum_angular_impulse / t);
}

Blade::~Blade()
{
	delete[] elems;
}

int main()
{
	Airfoil NACA_0012;
	NACA_0012.attachData("airfoil_NACA_0012.txt");
	
	Blade blade(6, 0.3, &NACA_0012, 20);
	blade.amplitude = 25;
	blade.collective = 15;
	blade.freq = 20;
	
	for(int i = 0; i < 1000; i++)
	{
		blade.update(Vec2(0, 0), 1.225, 0.001);
		std::cout << "t: " << blade.t;
		std::cout << "\tthrust: " << blade.thrust;
		std::cout << "\ttorque: " << blade.torque;
		std::cout << std::endl;
	}
	
	std::cout << "avg thrust: " << blade.getAvgThrust() << std::endl;
	std::cout << "avg torque: " << blade.getRMSTorque() << std::endl;
	
//	WingElement element[10];
//	for(int i = 0; i < 10; i++)
//		element[i].airfoil = &NACA_0012;
	
	
	return 0;
}
