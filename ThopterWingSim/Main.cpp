#include <iostream>
#include <cmath>
#include <vector>
#include "Airfoil.h"
#include "Blade.h"
#include "Vec2.h"
#include "BladeTester.h"

//void testFrequencies(Airfoil* airfoil)
//{
//	for(int f = 1; f < 31; f++)
//	{
//		double max_ratio = 0;
//		double collective_at_max = 0;
//		double thrust_at_max = 0;
//		double power_at_max = 0;
//		double prop_wash_at_max = 0;
//		double peak_torque_AC = 0;
//		double peak_lift_moment = 0;
//		for(int c = 1; c < 45; c++)
//		{
//			Blade blade(1, 0.1, 0.05, 10.0 / 216, airfoil, 20, 22.5);
////			blade.amplitude = 25;
//			blade.collective = c;
//			blade.freq = f;
//			for(int i = 0; i < 400; i++)
//			{
//				blade.update(Vec2(0, 0), 1.225, 1 / (100.0 * f));
//			}
//			
//			double ratio = blade.getAvgThrust() / blade.getAvgPower();
//			
//			if(ratio > max_ratio)
//			{
//				max_ratio = ratio;
//				collective_at_max = blade.collective;
//				thrust_at_max = blade.getAvgThrust();
//				power_at_max = blade.getAvgPower();
//				peak_torque_AC = blade.peak_torque_AC;
//				peak_lift_moment = blade.peak_lift_moment;
////				prop_wash_at_max = blade.getAvgPropWash();	
//			}
//		}
//		std::cout << "\nmax values at freq " << f << "Hz: " << std::endl;
//		std::cout << "collective: " << collective_at_max << std::endl;
//		std::cout << "avg thrust: " << thrust_at_max << std::endl;
//		std::cout << "avg power: " << power_at_max << "W, " << power_at_max / 746 << "hp" << std::endl;
//		std::cout << "N/w: " << max_ratio << std::endl;
//		std::cout << "g/w: " << max_ratio * (1000 / 9.8) << std::endl;
//		std::cout << "lbs/hp: " << max_ratio * (746 / 4.4444) << std::endl;
//		std::cout << "peak blade sweep-wise torque: " << peak_torque_AC << std::endl;
//		std::cout << "peak blade lift moment: " << peak_lift_moment << std::endl;
////		std::cout << "prop wash: " << prop_wash_at_max << std::endl;
//	}
//}
//
//void testChords(Airfoil* airfoil)
//{
//	for(double cr = 0.1; cr < 1.0; cr += 0.2) for(double ct = 0.1; ct < 1.0; ct += 0.2)
//	{
//		double max_ratio = 0;
//		double collective_at_max = 0;
//		double thrust_at_max = 0;
//		double power_at_max = 0;
//		double prop_wash_at_max = 0;
//		for(int c = 1; c < 45; c++)
//		{
//			Blade blade(6, cr, ct, 10, airfoil, 20, 25);
////			blade.amplitude = 25;
//			blade.collective = c;
//			blade.freq = 8;
//			for(int i = 0; i < 400; i++)
//			{
//				blade.update(Vec2(0, 0), 1.225, 1 / (100.0 * blade.freq));
//			}
//			
//			double ratio = blade.getAvgThrust() / blade.getAvgPower();
//			
//			if(ratio > max_ratio)
//			{
//				max_ratio = ratio;
//				collective_at_max = blade.collective;
//				thrust_at_max = blade.getAvgThrust();
//				power_at_max = blade.getAvgPower();
////				prop_wash_at_max = blade.getAvgPropWash();	
//			}
//		}
//		std::cout << "\nmax values at root chord " << cr << " and tip chord " << ct << std::endl;
//		std::cout << "collective: " << collective_at_max << std::endl;
//		std::cout << "avg thrust: " << thrust_at_max << std::endl;
//		std::cout << "avg power: " << power_at_max << std::endl;
//		std::cout << "N/w: " << max_ratio << std::endl;
//		std::cout << "lbs/hp: " << max_ratio * (746 / 4.4444) << std::endl;
////		std::cout << "prop wash: " << prop_wash_at_max << std::endl;
//	}
//}

std::vector<Blade*> saved_blades;
std::vector<Airfoil*> airfoils;

void addAirfoil()
{
	std::string name;
	std::cout << "\nEnter airfoil name: ";
	std::cin.ignore(1,'\n'); std::getline(std::cin, name);
	
	Airfoil* af = new Airfoil(name);
	bool checking_file = true;
	while(checking_file)
	{
		std::string fn;
		std::cout << "Enter file name: ";
		std::getline(std::cin, fn);
		if(!af->attachData(fn))
		{
			std::cout << "  file not found! ";
		}
		else
			checking_file = false;
	}
	airfoils.push_back(af);
	std::cout << "  Airfoil " << af->name << " saved!\n" << std::endl;	
}
bool airfoilSelector(Airfoil* af)
{
	if(!airfoils.size())
	{
		std::cout << "  There are no stored airfoils" << std::endl;
		return false;
	}
	std::cout << "Select an airfoil: " << std::endl;
	for(int i = 0; i < airfoils.size(); i++)
	{
		std::cout << "  " << i << ": " << airfoils[i]->name << std::endl;
	}
	int input; std::cin >> input;
	while(input < 0 || input >= airfoils.size())
	{
		std::cout << "invalid selection, try again: ";
		std::cin >> input;
	}
	af = airfoils[input];
	return true;
}
void airfoilLoop()
{
	char input;
	std::cout << "\nView saved airfoils (v), add new airfoil (a)... ";
	std::cin >> input;
	switch(input)
	{
		case 'v':
		{
			if(!airfoils.size())
			{
				std::cout << "There are no saved airfoils to view." << std::endl;
				break;
			}
			else
			{
				for(int i = 0; i < airfoils.size(); i++)
				{
					std::cout << "  " << i << ": " << airfoils[i]->name << std::endl;
				}
			}
			break;
		}
		case 'a':
		{
			addAirfoil();
			break;
		}
	}
}
void bladeLoop()
{
	char input;
	std::cout << "\nView saved blades (v), create new blade (c)... ";
	std::cin >> input;
	switch(input)
	{
		/// View stored blades
		case 'v':
		{
			if(!saved_blades.size())
			{
				std::cout << "There are no saved blades to view." << std::endl;
				break;
			}
			else
			{
				for(int i = 0; i < saved_blades.size(); i++)
				{
					saved_blades[i]->printInfo();
				}
			}
			break;
		}
		
		/// Create new blade
		case 'c':
		{
			double span, cr, ct, mass, amp;
			int num_elems;
			Airfoil* af;
			std::cout << "\n///// Blade Creator /////" << std::endl;
			std::cout << "  span: "; std::cin >> span;
			std::cout << "  root chord: "; std::cin >> cr;
			std::cout << "  tip chord: "; std::cin >> ct;
			std::cout << "  mass: "; std::cin >> mass;
			std::cout << "  swing angle: "; std::cin >> amp; amp /= 2;
			std::cout << "  number of wing elements: "; std::cin >> num_elems;
			if(!airfoilSelector(af)) break;
			
			saved_blades.push_back(new Blade(span, cr, ct, mass, af, num_elems, amp));
			std::cout << "Blade Saved!\n" << std::endl;
			break;
		}
	}
}

void testLoop()
{
	
}

int main()
{
	Airfoil NACA_0012;
	NACA_0012.attachData("airfoil_NACA_0012.txt");
	
	Blade blade(1, 0.1, 0.05, 10.0 / 216, &NACA_0012, 20, 22.5);
	blade.freq = 10;
	blade.collective = 14;
	
	PlotGenerator plotter;
	plotter.attachBlade(&blade);
	plotter.setIndepVar(&blade.freq, "frequency");
	plotter.setDepVar(&blade.specific_thrust, "specific Thrust");
	plotter.dep_output_scale = (746 / 4.448);
	plotter.run(1, 40, 1);
	
//	blade.collective = 80;
//	SimulateBlade(&blade, 1, 100, false);
//	blade.printInfo(true);
	
	
//	GradientAscent tester;
//	
//	Blade blade(1, 0.1, 0.05, 10.0 / 216, &NACA_0012, 20, 22.5);
//	blade.freq = 20;
//	
//	tester.attachBlade(&blade);
//	tester.sim_num_periods = 4;
//	tester.iters_per_period = 100;
//	
////	tester.addIndepVar(&blade.freq, 1, 15);
//	tester.addIndepVar(&blade.collective, 1, 60, "coll");
//	tester.addIndepVar(&blade.amplitude, 1, 60, "amp");
//	tester.setDepVar(&blade.specific_power, "spec_p");
//	tester.runTest(25, true, false);
	
//	bool running = true;
//	while(running)
//	{
//		char input;
//		std::cout << "Options: blades (b), tests (t), airfoils (a), quit (q)... ";
//		std::cin >> input;
//		switch(input)
//		{
//			case 'b':
//			{
//				bladeLoop();
//				break;
//			}
//			case 't':
//			{
//				testLoop();
//				break;
//			}
//			case 'a':
//			{
//				airfoilLoop();
//				break;
//			}
//			case 'q':
//			{
//				std::cout << "\n  Quitting...";
//				running = false;
//				break;
//			}
//		}
//		if(running) std::cout << "\n///// Main Menu /////" << std::endl;
//	}
	
//	BladeTester tester;
//	Blade blade(1, 0.1, 0.05, 10.0 / 216, &NACA_0012, 20, 22.5);
//	blade.collective = 22;
//	
//	tester.attachBlade(&blade);
//	tester.setTestParameters(&blade.freq, 20, 21, 1);
//	
//	tester.runTest(4, 100);
	
	
//	testFrequencies(&NACA_0012);
//	testChords(&NACA_0012);
	
//	for(int c = -5; c < 50; c++)
//	{
//		Blade blade(6, 1, 0.5, 10, &NACA_0012, 20, 25);
////		blade.amplitude = 25;
//		blade.collective = c;
//		blade.freq = 8;	
//	
//		for(int i = 0; i < 400; i++)
//		{
////			std::cout << "\n//////////////////Iteration: " << i << "///////////////////////////" << std::endl;
//			blade.update(Vec2(0, 0), 1.225, 1 / (100.0 * blade.freq), false);
////			blade.printDebug();
//			
//		}
//		
//		std::cout << "\nmax values at freq " << blade.freq << "Hz: " << std::endl;
//		std::cout << "collective: " << blade.collective << std::endl;
//		std::cout << "avg thrust: " << blade.getAvgThrust() << std::endl;
//		std::cout << "avg power: " << blade.getAvgPower() << std::endl;
//		double ratio = blade.getAvgThrust() / blade.getAvgPower();
//		std::cout << "N/w: " << ratio << std::endl;
//		std::cout << "lbs/hp: " << ratio * (746 / 4.4444) << std::endl;
////		std::cout << "prop wash: " << blade.disk_air_flow * 2 << std::endl;
//	}

//	Blade blade(6, 0.3, 0.1, 10, &NACA_0012, 20, 25);
////	blade.amplitude = 25;
//	blade.collective = 17;
//	blade.freq = 8;	
//
//	for(int i = 0; i < 200; i++)
//	{
//		blade.update(Vec2(-75, 0), 1.225, 1 / (100.0 * blade.freq), true);
//	}
//	
//	std::cout << "\nmax values at freq " << blade.freq << "Hz: " << std::endl;
//	std::cout << "collective: " << blade.collective << std::endl;
//	std::cout << "avg thrust: " << blade.getAvgThrust() << std::endl;
//	std::cout << "avg power: " << blade.getAvgPower() << "W, " << blade.getAvgPower() / 746 << "hp" << std::endl;
//	double ratio = blade.getAvgThrust() / blade.getAvgPower();
//	std::cout << "N/w: " << ratio << std::endl;
//	std::cout << "lbs/hp: " << ratio * (746 / 4.4444) << std::endl;
//	
//	blade.printRegions();
	
	return 0;
}
