#include "Blade.h"
#include "PiConsts.h"

Blade::Blade(double span, double chord, Airfoil* af, int num_elems) : span(span), chord(chord), num_elems(num_elems)
{
	elems = new WingElement[num_elems];
	for(int i = 0; i < num_elems; i++)
	{
		elems[i].airfoil = af;
		elems[i].chord = chord;
		elems[i].span = span / num_elems;
//		std::cout << i << ' ' << elems[i].chord << ' ' << elems[i].span << std::endl;
	}
//	std::cout << "num elems: " << num_elems << std::endl;
}

void Blade::printElems()
{
	std::cout << "elem info: " << std::endl;
	for(int i = 0; i < num_elems; i++)
	{
		std::cout << "i: " << i << "\tlift: " << elems[i].lift << "\tdrag: " << elems[i].drag << "\tangle: " << elems[i].angle << "\taoa: " << elems[i].aoa << "\tL/D: " << elems[i].cl / elems[i].cd << std::endl;
	}
}

void Blade::update(Vec2 airflow, double air_dens, double dt)
{
	double dir = getAvgThrust() < 0 ? -1 : 1;
	airflow = airflow + Vec2(0, dir * sqrt(2 * dir * getAvgThrust() / (air_dens * (pi * span * span * amplitude / 360)))); // steal thrust momentum from airflow
//	std::cout << "getavgthr: " << getAvgThrust() << std::endl;
	thrust = 0;
	torque = 0;
	t += dt;
	
	double w = freq * two_pi;
	double twist = collective * -cos(w * t);	// degrees
	double angular_vel = -amplitude * pi_over_180 * w * cos(w * t);	// rads
	for(int i = 0; i < num_elems; i++)
	{
		double radius = span * (double)i / num_elems;
		elems[i].angle = twist;
		elems[i].vel = Vec2(angular_vel * radius, 0);
		elems[i].calculateForces(airflow, air_dens);
		
		thrust += elems[i].force.y;
		torque += elems[i].force.x * radius;
	}
	
//	printElems();
	
	sum_impulse += thrust * dt;
	sum_energy_squared += torque * torque * w * w * dt;
}

double Blade::getAvgThrust()
{
	if(t == 0)
		return 0;
	return sum_impulse / t;
}

double Blade::getRMSPower()
{
	return sqrt(sum_energy_squared / t);
}

Blade::~Blade()
{
	delete[] elems;
}
