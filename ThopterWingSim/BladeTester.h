#ifndef BLADE_TESTER
#define BLADE_TESTER

#include "Blade.h"

class BladeTester
{
private:
	Blade* blade;
	
	double* test_param;
	double lower;
	double higher;
	int num_divisions;
	
public:
	BladeTester(){}
	BladeTester(Blade* blade, double* test_param, double lower, double higher, int num_divisions);
	
	void runTest(double num_periods, double iterations_per_period);
	void setTestParameter(double* test_param, double lower, double higher, int num_divisions);
};

#endif
