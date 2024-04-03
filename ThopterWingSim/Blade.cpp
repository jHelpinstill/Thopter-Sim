#include "Blade.h"
#include "PiConsts.h"

Blade::Blade(double span, double chord_root, double chord_tip, Airfoil* af, int num_elems, double amplitude)
{
	this->span = span;
	this->chord_root = chord_root;
	this->chord_tip = chord_tip;
	this->num_elems = num_elems;
	this->amplitude = amplitude;
	
	elems = new WingElement[num_elems];
	regions = new SweptRegion*[num_elems];
	double mass_area_density = mass / ((chord_root + chord_tip) * span / 2);
	for(int i = 0; i < num_elems; i++)
	{
		regions[i] = new SweptRegion[regions_per_elem];
		
		elems[i].airfoil = af;
		double chord_ratio = (double)i / num_elems;
		elems[i].chord = (1 - chord_ratio) * chord_root + chord_ratio * chord_tip;
		elems[i].span = span / num_elems;
		elems[i].mass = elems[i].chord * elems[i].span * mass_area_density;
		
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
	torque_AC = 0;
	double lift_moment = 0;
	t += dt;
	
	double w = freq * two_pi;
	double twist = collective * cos(w * t);	// degrees
	
	double position = 0.5 * (sin(w * t) + 1);
	double s = (position * (double)regions_per_elem);
	int airflow_region = (int)(s - 0.5);
	
	bool under = (s < 0.5);
	bool over = (s >= (regions_per_elem - 1 + 0.5));
	if(!under)
		s -= airflow_region;
	s -= 0.5;
	
//	std::cout << "\n///// BLADE UPDATE: s = " << s << "\tairflow region: " << airflow_region << "\tstate: ";
//	if(under) std::cout << "under" << std::endl;
//	else if(over) std::cout << "over" << std::endl;
//	else std::cout << "normal" << std::endl;
	
	if(print_elems)
	{
		printRegions();
		printElems();
	}
	
	double angular_vel = amplitude * pi_over_180 * w * cos(w * t);	// rads / s
	double angular_accel = amplitude * pi_over_180 * w * w * -sin(w * t); // rads / s^2
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
		lift_moment += elems[i].force.y * radius;
		torque_AC += elems[i].mass * radius * angular_accel;
	}
	
	if(print_elems) std::cout << "total blade thrust: " << thrust << "\ttotal blade torque: " << torque << "\tinstantaneous power: " << torque * w << "W, " << torque * w / 746 << "hp" << std::endl;
	if(torque > peak_torque)
		peak_torque = torque;
	if(lift_moment > peak_lift_moment)
		peak_lift_moment = lift_moment;
	if(torque_AC > peak_torque_AC)
		peak_torque_AC = torque_AC;
	sum_impulse += thrust * dt;
	sum_energy_squared += torque * w * dt;
}

double Blade::getAvgThrust()
{
	if(t == 0)
		return 0;
	return sum_impulse / t;
}

double Blade::getRMSPower()
{
	return sum_energy_squared / t;
}

void Blade::printDebug()
{
	std::cout << "Blade debug info: " << std::endl;
	std::cout << "num_elems: " << num_elems;
	std::cout << "\tregions_per_elem " << regions_per_elem;
	std::cout << "\tspan: " << span;
	std::cout << "\tchord_root: " << chord_root;
	std::cout << "\tchord_tip: " << chord_tip;
	std::cout << "\tsum_immpulse: " << sum_impulse;
	std::cout << "\tsum_energy_squared: " << sum_energy_squared;
	
	std::cout << "\tt: " << t;
	
	// swing attributes (deg, deg, Hz);
	std::cout << "\tamplitude: " << amplitude;
	std::cout << "\tcollective: " << collective;
	std::cout << "\tfreq: " << freq;
	
	std::cout << "\tthrust: " << thrust;
	std::cout << "\ttorque: " << torque;
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
