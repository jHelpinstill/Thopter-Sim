#ifndef BLADE_TESTER
#define BLADE_TESTER

#include "Blade.h"
#include <vector>
#include <string>
#include <cmath>

struct Variable
{
	std::string name;
	Variable(){}
	Variable(double* param, double lower, double upper, std::string name = "") : 
		param(param), lower(lower), upper(upper), name(name), at_bounds(false), bounds_active(true) {}
	Variable(double* param, std::string name = "") : 
		param(param), name(name), bounds_active(false) {}
		
	double* param;
	
	double lower;
	double upper;
	bool at_bounds;
	bool bounds_active;
	
	void setValue(double val)
	{
		if(bounds_active)
		{
			at_bounds = false;
			if(val > upper)
				{ val = upper; at_bounds = true; }
			else if(val < lower)
				{ val = lower; at_bounds = true; }
		}
		*param = val;
	}
	void adjustValue(double step)
	{
		if(bounds_active) setValue(*param + (upper - lower) * step);
		else setValue(*param + step);
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
	ResultData(double indep_var, double dep_var) : indep_var(indep_var), dep_var(dep_var) {}
	ResultData(std::vector<Variable>& vars, double dep_var) : dep_var(dep_var)
	{
		addIndep(vars);
	}
	
	std::vector<double> indep_vars;
	double indep_var;
	double dep_var;
	
	void addIndep(std::vector<Variable>& vars)
	{
		for(int i = 0; i < vars.size(); i++)
			indep_vars.push_back(vars[i].getValue());
	}
};

class GradientAscent
{
private:
	Blade* blade;
	
	std::vector<Variable> indep_vars;
	Variable dep_var;
	std::vector<ResultData> results;
	
	void runSim();
	
public:
	GradientAscent() : sim_num_periods(4), iters_per_period(100) {}
	
	double sim_num_periods;
	double iters_per_period;
	
	void attachBlade(Blade* blade);
	void addIndepVar(double* param, double lower, double upper, std::string name = "var");
	void setDepVar(double* param, std::string name = "var");
	void runTest(int num_rebounds, bool start_midpoint = true, bool verbose = false);	// gradient descent maximizer
	
	void printResultsCSV();
};

class PlotGenerator
{
private:
	Blade* blade;
	Variable indep_var;
	Variable dep_var;
	
	std::vector<ResultData> results;
	
	void runSim();
	
public:
	PlotGenerator() : 
		sim_num_periods(4),
		iters_per_period(100),
		indep_output_scale(1),
		dep_output_scale(1)
	{}
	
	void run(bool verbose = false);
	void run(double from, double to, double incr, bool verbose = false);
	
	double from;
	double to;
	double incr;
	void setAttributes(double from, double to, double incr);
	
	double sim_num_periods;
	double iters_per_period;
	double indep_output_scale;
	double dep_output_scale;
	
	void attachBlade(Blade* blade);
	void setIndepVar(double* param, std::string name = "var");
	void setDepVar(double* param, std::string name = "var");
	void printResultsCSV();
};

void SimulateBlade(Blade* blade, int num_periods, int iters_per_period, bool verbose = false);
//{
//	blade->reset();
//	
//	double period = 1 / blade->freq;
//	double dt = period / iters_per_period;
//	int iterations = iters_per_period * num_periods;
//	for(int i = 0; i < iterations; i++)
//	{
//		blade->update(Vec2(0, 0), 1.225, dt, verbose);
//	}
//}

#endif
