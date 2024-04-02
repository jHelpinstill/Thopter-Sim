#include "Blade.h"
#include "PiConsts.h"

Blade::Blade(double span, double chord, Airfoil* af, int num_elems, double amplitude) : span(span), chord(chord), num_elems(num_elems), amplitude(amplitude)
{
	elems = new WingElement[num_elems];
	regions = new SweptRegion*[num_elems];
	for(int i = 0; i < num_elems; i++)
	{
		regions[i] = new SweptRegion[regions_per_elem];
		
		elems[i].airfoil = af;
		elems[i].chord = chord;
		elems[i].span = span / num_elems;
//		std::cout << i << ' ' << elems[i].chord << ' ' << elems[i].span << std::endl;
		for(int r = 0; r < regions_per_elem; r++)
			regions[i][r].area = (elems[i].span * (span * ((double)i + 0.5) / num_elems) * amplitude * pi_over_180) / regions_per_elem;
	}
//	std::cout << "num elems: " << num_elems << std::endl;
}

void Blade::printElems()
{
	std::cout << "elem info: " << std::endl;	
	for(int i = 0; i < num_elems; i++)
	{
		std::cout << "i: " << i << "\tlift: " << elems[i].lift << "\tdrag: " << elems[i].drag << "\tangle: " << elems[i].angle << "\taoa: " << elems[i].aoa << "\tL/D: " << elems[i].cl / elems[i].cd << "\tthrust: " << elems[i].force.y << "\ttorque: " << elems[i].force.x * span * (double)i / num_elems << std::endl;
	}
}

void Blade::printRegions()
{
	std::cout << "region info: " << std::endl;
	for(int i = 0; i < num_elems; i++)
	{
		std::cout << "i: " << i << "\twash: ";
		for(int j = 0; j < regions_per_elem; j++)
		{
			std::cout << regions[i][j].wash << '\t';
		}
		std::cout << std::endl;
	}
}

void Blade::update(Vec2 airflow, double air_dens, double dt, bool print_elems)
{
//	std::cout << "getavgthr: " << getAvgThrust() << std::endl;
	thrust = 0;
	torque = 0;
	t += dt;
	
	double w = freq * two_pi;
	double twist = collective * cos(w * t);	// degrees
	
	double position = 0.5 * (sin(w * t) + 1);
	double s = (position * (double)regions_per_elem);
	int airflow_region = int(s - 0.5);
	
	bool under = (s < 0.5);
	bool over = (s >= regions_per_elem + 0.5);
	if(!under)
		s -= airflow_region;
	s -= 0.5;
	
	double angular_vel = amplitude * pi_over_180 * w * cos(w * t);	// rads
	for(int i = 0; i < num_elems; i++)
	{
		double radius = span * (double)i / num_elems;
		
		elems[i].angle = twist;
		elems[i].vel = Vec2(angular_vel * radius, 0);
		
		Vec2 diskVel;
		if(under)
			diskVel = regions[i][0].getDiskVel(t, air_dens) * (2 * s);
		else if(over)
			diskVel = regions[i][regions_per_elem - 1].getDiskVel(t, air_dens) * (1 - 2 * s);
		else
			diskVel = (regions[i][airflow_region].getDiskVel(t, air_dens) * (1 - s)) + (regions[i][airflow_region + 1].getDiskVel(t, air_dens) * s);
		elems[i].calculateForces(airflow + diskVel, air_dens);
		
		if(under)
			regions[i][0].addThrust(2 * s * elems[i].force.y, dt);
		else if(over)
			regions[i][regions_per_elem - 1].addThrust((1 - 2 * s) * elems[i].force.y, dt);
		else
		{
			regions[i][airflow_region].addThrust((1 - s) * elems[i].force.y, dt);
			regions[i][airflow_region + 1].addThrust(s * elems[i].force.y, dt);
		}
		thrust += elems[i].force.y;
		torque += elems[i].force.x * radius;
	}
	
	if(print_elems)
	{
//		printRegions();
		printElems();
		std::cout << "total blade thrust: " << thrust << "\ttotal blade torque: " << torque << std::endl;
	}
	
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

//double Blade::getAvgPropWash()
//{
//	double total_area = 0;
//	double sum = 0;
//	for(int i = 0; i < num_elems; i++)
//	{
//		total_area += regions[i].area;
//		sum += regions[i].wash * regions[i].area;
//	}
//	return 2 * sum / total_area;
//}

Blade::~Blade()
{
	delete[] elems;
	for(int i = 0; i < num_elems; i++)
		delete[] regions[i];
}
