#include "BladeTester.h"

BladeTester::BladeTester(Blade* blade, double* test_param, double lower, double higher, int num_divisions)
{
	this->blade = blade;
	setTestParameter(test_param, lower, higher, num_divisions);
}
void BladeTester::runTest(double num_periods, double iterations_per_period)
{
	double increment = (higher - lower) / num_divisions;
	*test_param = lower;
	for(int run = 0; run < num_divisions; run++)
	{
		blade->reset();
		double period = 1 / blade->freq;
		double dt = period / iterations_per_period;
		int iterations = iterations_per_period * num_periods;
		for(int i = 0; i < iterations; i++)
		{
			blade->update(Vec2(-75, 0), 1.225, dt);
		}
		
		std::cout << "Test results: " << std::endl;
		std::cout << "Freq: " << blade->freq << "Hz: " << std::endl;
		std::cout << "collective: " << blade->collective << std::endl;
		std::cout << "avg thrust: " << blade->getAvgThrust() << std::endl;
		std::cout << "avg power: " << blade->getAvgPower() << "W, " << blade->getAvgPower() / 746 << "hp" << std::endl;
		double ratio = blade->getAvgThrust() / blade->getAvgPower();
		std::cout << "N/w: " << ratio << std::endl;
		std::cout << "lbs/hp: " << ratio * (746 / 4.4444) << std::endl;
		
		*test_param += increment;
	}
}
void BladeTester::setTestParameter(double* test_param, double lower, double higher, int num_divisions)
{
	this->test_param = test_param;
	this->lower = lower;
	this->higher = higher;
	this->num_divisions = num_divisions;
}
