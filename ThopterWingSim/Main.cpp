#include <iostream>
#include <cmath>
#include "Airfoil.h"
#include "Blade.h"
#include "Vec2.h"

void testFrequencies(Airfoil* airfoil)
{
	for(int f = 1; f < 30; f++)
	{
		double max_ratio = 0;
		double collective_at_max = 0;
		double thrust_at_max = 0;
		double power_at_max = 0;
		double prop_wash_at_max = 0;
		for(int c = -10; c < 45; c++)
		{
			Blade blade(6, 0.5, airfoil, 20, 25);
//			blade.amplitude = 25;
			blade.collective = c;
			blade.freq = f;
			for(int i = 0; i < 200; i++)
			{
				blade.update(Vec2(0, 0), 1.225, 1 / (100.0 * f));
			}
			
			double ratio = blade.getAvgThrust() / blade.getRMSPower();
			
			if(ratio > max_ratio)
			{
				max_ratio = ratio;
				collective_at_max = blade.collective;
				thrust_at_max = blade.getAvgThrust();
				power_at_max = blade.getRMSPower();
//				prop_wash_at_max = blade.getAvgPropWash();	
			}
		}
		std::cout << "\nmax values at freq " << f << "Hz: " << std::endl;
		std::cout << "collective: " << collective_at_max << std::endl;
		std::cout << "avg thrust: " << thrust_at_max << std::endl;
		std::cout << "avg power: " << power_at_max << std::endl;
		std::cout << "N/w: " << max_ratio << std::endl;
		std::cout << "lbs/hp: " << max_ratio * (746 / 4.4444) << std::endl;
//		std::cout << "prop wash: " << prop_wash_at_max << std::endl;
	}
}

int main()
{	
	Airfoil NACA_0012;
	NACA_0012.attachData("airfoil_NACA_0012.txt");
	
//	testFrequencies(&NACA_0012);
	
//	for(int c = -5; c < 50; c++)
//	{
//		Blade blade(6, 0.3, &NACA_0012, 20);
//		blade.amplitude = 25;
//		blade.collective = c;
//		blade.freq = 8;	
//	
//		for(int i = 0; i < 200; i++)
//		{
//			blade.update(Vec2(0, 0), 1.225, 1 / (100.0 * blade.freq));
//		}
//		
//		std::cout << "\nmax values at freq " << blade.freq << "Hz: " << std::endl;
//		std::cout << "collective: " << blade.collective << std::endl;
//		std::cout << "avg thrust: " << blade.getAvgThrust() << std::endl;
//		std::cout << "avg power: " << blade.getRMSPower() << std::endl;
//		double ratio = blade.getAvgThrust() / blade.getRMSPower();
//		std::cout << "N/w: " << ratio << std::endl;
//		std::cout << "lbs/hp: " << ratio * (746 / 4.4444) << std::endl;
//		std::cout << "prop wash: " << blade.disk_air_flow * 2 << std::endl;
//	}

	Blade blade(6, 0.3, &NACA_0012, 20, 25);
//	blade.amplitude = 25;
	blade.collective = 20;
	blade.freq = 8;	

	for(int i = 0; i < 200; i++)
	{
		blade.update(Vec2(0, 0), 1.225, 1 / (100.0 * blade.freq), true);
	}
	
	std::cout << "\nmax values at freq " << blade.freq << "Hz: " << std::endl;
	std::cout << "collective: " << blade.collective << std::endl;
	std::cout << "avg thrust: " << blade.getAvgThrust() << std::endl;
	std::cout << "avg power: " << blade.getRMSPower() << std::endl;
	double ratio = blade.getAvgThrust() / blade.getRMSPower();
	std::cout << "N/w: " << ratio << std::endl;
	std::cout << "lbs/hp: " << ratio * (746 / 4.4444) << std::endl;
	
	blade.printRegions();
	
	return 0;
}
