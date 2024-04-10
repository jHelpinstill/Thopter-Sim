#ifndef BLADE_TESTER
#define BLADE_TESTER

#include "Blade.h"
#include <vector>
#include <string>
#include <cmath>

struct IndepVar
{
	std::string name;
	IndepVar(){}
	IndepVar(double* param, double lower, double upper, std::string name = "") : 
		param(param), lower(lower), upper(upper), name(name), walk_step(0.2), at_bounds(false) {}
		
	double* param;
	
	double lower;
	double upper;
	double walk_step;
	bool at_bounds;
	
	void setValue(double val)
	{
		at_bounds = false;
		if(val > upper)
			{ val = upper; at_bounds = true; }
		else if(val < lower)
			{ val = lower; at_bounds = true; }
		*param = val;
	}
	void adjustValue(double step)
	{
		setValue(*param + (upper - lower) * step);
	}
	double getValue()
	{
		return *param;
	}
	void print()
	{
		std::cout << name << ": " << *param;
	}
};

struct DepVar
{
	std::string name;
	DepVar(){}
	DepVar(double* param, std::string name = "") : param(param), name(name), prev_val(0) {}
	
	double* param;
	double prev_val;
	
	double getValue()
	{
		return *param;
	}
	void print()
	{
		std::cout << name << ": " << *param;
	}
};

struct StateVector
{
	StateVector(){}
	StateVector(int dimensions)
	{
		for(int i = 0; i < dimensions; i++)
			axes.push_back(0);
	}
	std::vector<double> axes;
	double mag()
	{
		double sum_of_squares = 0;
		for(int i = 0; i < axes.size(); i++)
			sum_of_squares += axes[i] * axes[i];
		return sqrt(sum_of_squares);
	}
	StateVector unit()
	{
		StateVector u;
		double magnitude = mag();
		for(int i = 0; i < axes.size(); i++)
			u.axes.push_back(axes[i] / magnitude);
		return u;
	}
	StateVector operator*(double a)
	{
		StateVector u;
		for(int i = 0; i < axes.size(); i++)
			u.axes.push_back(axes[i] * a);
		return u; 
	}
};

struct ResultData
{
	ResultData(){}
	ResultData(std::vector<IndepVar>& vars, double dep_var) : dep_var(dep_var)
	{
		addIndep(vars);
	}
	std::vector<double> indep_vars;
	double dep_var;
	
	void addIndep(std::vector<IndepVar>& vars)
	{
		for(int i = 0; i < vars.size(); i++)
			indep_vars.push_back(vars[i].getValue());
	}
};

class GradientAscent
{
private:
	Blade* blade;
	
	std::vector<IndepVar> indep_vars;
	DepVar dep_var;
	std::vector<ResultData> results;
	
	void runSim();
	
public:
	GradientAscent(){}
	
	double sim_num_periods;
	double iters_per_period;
	
	void attachBlade(Blade* blade);
	void addIndepVar(double* param, double lower, double upper, std::string name = "var");
	void setDepVar(double* param, std::string name = "var");
	void runTest(int num_rebounds, bool start_midpoint = true, bool verbose = false);	// gradient descent maximizer
	
	void printResultsCSV();
//	void setTestParameters(double* test_param, double lower, double higher, int num_divisions);
};

#endif
