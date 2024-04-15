#include "Blade.h"
#include "PiConsts.h"

Blade::Blade(double span, double chord_root, double chord_tip, double mass, Airfoil* af, int num_elems, double amplitude) :
	span(span),
	chord_root(chord_root),
	chord_tip(chord_tip),
	mass(mass),
	amplitude(amplitude),
	num_elems(num_elems),
	airfoil(af)
{
	build();
}

void Blade::rebuild()
{
	delete[] elems;
	for(int i = 0; i < num_elems; i++)
		delete[] regions[i];
	
	build();
}

void Blade::build()
{	
	elems = new WingElement[num_elems];
	regions = new SweptRegion*[num_elems];
	double mass_area_density = mass / ((chord_root + chord_tip) * span / 2);
	for(int i = 0; i < num_elems; i++)
	{
		regions[i] = new SweptRegion[regions_per_elem];
		
		elems[i].airfoil = airfoil;
		double chord_ratio = (double)i / num_elems;
		elems[i].chord = (1 - chord_ratio) * chord_root + chord_ratio * chord_tip;
		elems[i].span = span / num_elems;
		elems[i].mass = elems[i].chord * elems[i].span * mass_area_density;
		
		for(int r = 0; r < regions_per_elem; r++)
			regions[i][r].area = (elems[i].span * (span * ((double)i + 0.5) / num_elems) * amplitude * pi_over_180) / regions_per_elem;
	}
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
			std::cout << regions[i][j].wash.mag() << '\t';
		}
		std::cout << std::endl;
	}
}

void Blade::update(Vec2 airflow, double air_dens, double dt, bool print_elems)
{
//	std::cout << "getavgthr: " << getAvgThrust() << std::endl;
	force = Vec2(0, 0);
	axial_thrust = 0;
	transverse_thrust = 0;
	torque = 0;
	transverse_moment = 0;
	lift_moment = 0;
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
	
	angular_position = sin(w * t) * amplitude; // deg
	double angular_vel = amplitude * pi_over_180 * w * cos(w * t);	// rads / s
	double angular_accel = amplitude * pi_over_180 * w * w * -sin(w * t); // rads / s^2
	if(print_elems)
	{
		std::cout << "blade position: " << sin(w * t) * amplitude << "deg";
		std::cout << "\tblade velocity: " << angular_vel << "rad/s, " << angular_vel / pi_over_180 << "deg/s";
		std::cout << "\tblade accel: " << angular_accel << "rad/s^2, " << angular_accel / pi_over_180 << "deg/s^2" << std::endl;
		printRegions();
		printElems();
	}
	airflow.rotate(-sweep_plane_angle);
	
	for(int i = 0; i < num_elems; i++)
	{
		double radius = span * (double)i / num_elems;
		
		elems[i].angle = twist - sweep_plane_angle;
		elems[i].vel = Vec2(angular_vel * radius, 0);
		
		Vec2 disk_vel;
		if(under)
			disk_vel = regions[i][0].getDiskVel(t, air_dens) * (2 * s);
		else if(over)
			disk_vel = regions[i][regions_per_elem - 1].getDiskVel(t, air_dens) * (1 - 2 * s);
		else
			disk_vel = (regions[i][airflow_region].getDiskVel(t, air_dens) * (1 - s)) + (regions[i][airflow_region + 1].getDiskVel(t, air_dens) * s);
			
		elems[i].calculateForces(airflow + disk_vel, air_dens);
		
		if(under)
			regions[i][0].addThrust(elems[i].force * 2 * s, dt);
		else if(over)
			regions[i][regions_per_elem - 1].addThrust(elems[i].force * (1 - 2 * s), dt);
		else
		{
			regions[i][airflow_region].addThrust(elems[i].force * (1 - s), dt);
			regions[i][airflow_region + 1].addThrust(elems[i].force * s, dt);
		}
		axial_thrust += elems[i].force.y;
		transverse_thrust += elems[i].force.x;
		torque += elems[i].force.x * radius;
		lift_moment += elems[i].force.y * radius;
		transverse_moment += (elems[i].force.x * radius) - (elems[i].mass * radius * angular_accel);
	}
	
	transverse_thrust *= cos(angular_position * pi_over_180);
	force = Vec2(transverse_thrust, axial_thrust).rotate(sweep_plane_angle * pi_over_180);
	
	power = torque * angular_vel;
	specific_thrust = axial_thrust / power;
	
	if(torque > peak_torque)
		peak_torque = torque;
	if(lift_moment > peak_lift_moment)
		peak_lift_moment = lift_moment;
	if(transverse_moment > peak_transverse_moment)
		peak_transverse_moment = transverse_moment;
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
	std::cout << "\tsum_energy: " << sum_energy;
	
	std::cout << "\tt: " << t;
	
	// swing attributes (deg, deg, Hz);
	std::cout << "\tamplitude: " << amplitude;
	std::cout << "\tcollective: " << collective;
	std::cout << "\tfreq: " << freq;
	
	std::cout << "\tthrust: " << axial_thrust;
	std::cout << "\ttorque: " << torque;
}

void Blade::printInfo(bool verbose)
{
	std::cout <<
	"\nDimensions:\n  span: " << span <<
	"\n  chord root:\t" << chord_root << 
	"\n  chord tip:\t" << chord_tip << 
	"\n  mass:\t" << mass <<
	"\n  spec thrust:\t" << specific_thrust << "N/w, " << specific_thrust * (1000 / 9.8) << "g/w, " << specific_thrust * (746 / 4.448) << "lbs/hp";
	
	if(verbose)
	{
		std::cout << "\nOther info: " <<
		"num_elems: " << num_elems <<
		"\nregions_per_elem " << regions_per_elem <<
		"\nspan: " << span <<
		"\nchord_root: " << chord_root <<
		"\nchord_tip: " << chord_tip <<
		"\namplitude: " << amplitude <<
		"\ncollective: " << collective <<
		"\nfreq: " << freq;
	}
	
	std::cout << std::endl;
}

void Blade::reset()
{
	t = 0;
	sum_impulse = 0;
	sum_energy = 0;
	
	peak_torque = 0;
	peak_transverse_moment = 0;
	peak_lift_moment = 0;
}

Blade::~Blade()
{
	delete[] elems;
	for(int i = 0; i < num_elems; i++)
		delete[] regions[i];
}
