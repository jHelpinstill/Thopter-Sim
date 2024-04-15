#ifndef BLADE_TESTER
#define BLADE_TESTER

#include "Blade.h"
#include <vector>
#include <string>
#include <cmath>

struct Variable
{
	std::string name;
	Variable() : output_scale(1) {}
	// independent varible with bounds
	Variable(double* param, double lower, double upper, std::string name = "") : 
		param(param), lower(lower), upper(upper), name(name), at_bounds(false), bounds_active(true), output_scale(1), ratio(false), average(false) {}
		
	// dependent variable, can be instantaneous or average, for avg initial periods can be skipped over to allow for settling
	Variable(double* param, std::string name = "", double output_scale = 1, bool average = false, int T_to_skip = 0) : 
		param(param), name(name), bounds_active(false), sum(0), t(0), output_scale(output_scale), average(average), ratio(false), post_divide(false), T_to_skip(T_to_skip) {}
		
	// ratio of dependent variables, division can be done on either the instantaneous or averaged values
	Variable(double* param, double* param2, std::string name = "", double output_scale = 1, bool average = false, bool post_divide = false, int T_to_skip = 0) : 
		param(param), param2(param2), name(name), bounds_active(false), sum(0), sum2(0), t(0), output_scale(output_scale), average(average), ratio(true), post_divide(post_divide), T_to_skip(T_to_skip) {}
		
	double* param;
	double* param2;
	
	bool average;
	int T_to_skip;
	bool ratio;
	bool post_divide;
	double sum;
	double sum2;
	double t;
	double output_scale;
	
	double lower;
	double upper;
	bool at_bounds;
	bool bounds_active;
	
	void reset()
	{
		sum = 0;
		sum2 = 0;
	}
	void setValue(double val)
	{
		if(ratio) return;
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
		if(ratio) return;
		if(bounds_active) setValue(*param + (upper - lower) * step);
		else setValue(*param + step);
	}
	void recordAvg(double dt)
	{
		if(ratio)
		{
			if(post_divide)
			{
				sum += *param * dt;
				sum2 += *param2 * dt;
			}
			else
				sum += (*param / *param2) * dt;
		}
		else
		{
			sum += *param * dt;
		}
		t += dt;
	}
	double getValue()
	{
		if(average)
		{
			if(post_divide)
				return (sum / sum2);
			return sum / t;
		}
		if(ratio)
			return *param / *param2;
		return *param;
	}
	void print()
	{
		std::cout << name << ": " << getValue();
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

struct ResultDatum
{
	ResultDatum(){}
	ResultDatum(double indep_var, double dep_var)
	{
		addIndep(indep_var);
		addDep(dep_var);
	}
	ResultDatum(std::vector<Variable>& indep_vars, double dep_var)
	{
		addIndep(indep_vars);
		addDep(dep_var);
	}
	ResultDatum(double indep_var, std::vector<Variable>& dep_vars)
	{
		addIndep(indep_var);
		addDep(dep_vars);
	}
	ResultDatum(std::vector<Variable>& indep_vars, std::vector<Variable>& dep_vars)
	{
		addIndep(indep_vars);
		addDep(dep_vars);
	}
	
	std::vector<double> indep_vars;
	std::vector<double> dep_vars;
	
	void addIndep(std::vector<Variable>& vars)
	{
		for(auto& v : vars)
			indep_vars.push_back(v.getValue());
	} 
	void addDep(std::vector<Variable>& vars)
	{
		for(auto& v : vars)	
			dep_vars.push_back(v.getValue() * v.output_scale);
	}
	void addIndep(double var)
	{
		indep_vars.push_back(var);
	}
	void addDep(double var)
	{
		dep_vars.push_back(var);
	}
};

class GradientAscent
{
private:
	Blade* blade;
	
	std::vector<Variable> indep_vars;
	Variable dep_var;
	std::vector<ResultDatum> results;
	
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
	std::vector<Variable> dep_vars;
	
	std::vector<ResultDatum> results;
	
	void runSim();
	
public:
	PlotGenerator() : 
		sim_num_periods(4),
		iters_per_period(100),
		indep_output_scale(1)
	{}
	
	void run(bool verbose = false);
	void run(double from, double to, double incr, bool verbose = false);
	void runRealTime(double periods_to_skip_over, int sampling_freq = 1);
	
	double from;
	double to;
	double incr;
	void setAttributes(double from, double to, double incr);
	
	double sim_num_periods;
	double iters_per_period;
	double indep_output_scale;
	
	void attachBlade(Blade* blade);
	void setIndepVar(double* param, std::string name = "var");
	void addDepVar(double* param, std::string name = "var", double output_scale = 1, bool average = false, int T_to_skip = 0);
	void addDepVar(double* param, double* param2, std::string name = "var", double output_scale = 1, bool average = false, bool post_divide = false, int T_to_skip = 0);
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
