#include <iostream>
#include <cmath>
#include "Airfoil.h"
#include "Blade.h"
#include "Vec2.h"

int main()
{
//	Airfoil test;
//	test.attachData("airfoil_test.txt", true);
//	
//	for(double i = -5; i < 5.1; i += 0.5)
//	{
//		std::cout << "cl @ " << i << ": " << test.getCl(i) << std::endl;
//	}
	
	Airfoil NACA_0012;
	NACA_0012.attachData("airfoil_NACA_0012.txt");
	for(int f = 1; f < 30; f++)
	{
		double max_ratio = 0;
		double collective_at_max = 0;
		double thrust_at_max = 0;
		double power_at_max = 0;
		for(int c = -10; c < 45; c++)
		{
			Blade blade(6, 0.3, &NACA_0012, 20);
			blade.amplitude = 25;
			blade.collective = c;
			blade.freq = f;	
			for(int i = 0; i < 1000; i++)
			{
				blade.update(Vec2(0, 0), 1.225, 0.001);
			}
			
			double ratio = blade.getAvgThrust() / blade.getRMSPower();
			
			if(ratio > max_ratio)
			{
				max_ratio = ratio;
				collective_at_max = blade.collective;
				thrust_at_max = blade.getAvgThrust();
				power_at_max = blade.getRMSPower();				
			}
		}
		std::cout << "max values at freq " << f << ": " << std::endl;
		std::cout << "\ncollective: " << collective_at_max << std::endl;
		std::cout << "avg thrust: " << thrust_at_max << std::endl;
		std::cout << "avg power: " << power_at_max << std::endl;
		std::cout << "N/w: " << max_ratio << std::endl;
		std::cout << "lbs/hp: " << max_ratio * (746 / 4.4444) << std::endl;
	}
//	WingElement element[10];
//	for(int i = 0; i < 10; i++)
//		element[i].airfoil = &NACA_0012;
	
	
	return 0;
}
