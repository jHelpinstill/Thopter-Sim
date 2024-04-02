#include <iostream>
#include <cmath>
#include "Airfoil.h"
#include "Vec2.h"

const double pi_over_180 = 3.14159265 / 180;
const double two_pi = 3.14159265 * 2;

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
	
	double t = 0;
	
	double sum_impulse = 0;
	double sum_angular_impulse = 0;
	
public:
	Blade(){}
	Blade(double span, double chord, Airfoil* af, int num_elems);
	
	// swing attributes;
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
	t += dt;
	
	thrust = 0;
	torque = 0;
	
	double w = freq * two_pi;
	double twist = collective * cos(w * t);	// degrees
	double angular_vel = -amplitude * pi_over_180 * w * cos(w * t);	// rads
	for(int i = 0; i < num_elems; i++)
	{
		elems[i].angle = twist;
		elems[i].vel = Vec2(angular_vel * span * (1 - (double)i / num_elems), 0);
		elems[i].calculateForces(airflow, air_dens);
		
		thrust += elems[i].force.y;
		torque += elems[i].force.x;
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
	
	WingElement e;
	e.airfoil = &NACA_0012;
	e.chord = 1;
	e.span = 0.1;
	
	for(int i = 0; i < 100; i++)
	{
		e.calculateForces(Vec2(-100, 0), 1.225);
		
		std::cout << "lift:\t" << e.lift << std::endl;
		std::cout << "drag:\t" << e.drag << std::endl;
		std::cout << "cl:\t" << e.cl << std::endl;
		std::cout << "cd:\t" << e.cd << std::endl;
		std::cout << "cl/cd:\t" << e.cl / e.cd<< std::endl;
		std::cout << "aoa:\t" << e.aoa << std::endl;
		std::cout << "angle:\t" << e.angle << std::endl;
		std::cout << "force vec:\t" << e.force.x << ", " << e.force.y << std::endl;
		
		e.angle += 0.5;
		std::cout << std::endl;
	}
	
//	WingElement element[10];
//	for(int i = 0; i < 10; i++)
//		element[i].airfoil = &NACA_0012;
	
	
	return 0;
}
