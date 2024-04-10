#include "BladeTester.h"

void BladeTester::runSim()
{
	blade->reset();
	blade->rebuild();
	
	double period = 1 / blade->freq;
	double dt = period / iters_per_period;
	int iterations = iters_per_period * sim_num_periods;
//	std::cout << "iterations: " << iterations << "\tperiod: " << period << "\tdt: " << dt << std::endl;
	for(int i = 0; i < iterations; i++)
	{
		blade->update(Vec2(0, 0), 1.225, dt);
	}
}
void BladeTester::runTest(int num_rebounds, bool start_midpoint, bool verbose)
{
	double search_step = 0.05; // step size when adjusting dependent variables
	double walk_step = 1;
	if(start_midpoint) for(auto& var : indep_vars)
	{	// set independent variables to their allowed midpoint
		var.setValue((var.upper + var.lower) / 2);
	}
	runSim();
	
	StateVector gradient(indep_vars.size());
	StateVector prev_position(indep_vars.size());
	int iteration = 0;
	int halves = num_rebounds;
	while(halves >= 0)
	{
		dep_var.prev_val = dep_var.getValue();
		
		// Estimate partial derivatives for each independent variable, and update gradient vector
		for(int i = 0; i < indep_vars.size(); i++)
		{
			prev_position.axes[i] = indep_vars[i].getValue();
			
			indep_vars[i].adjustValue(search_step);	// move indep var forward by one step
			runSim();
			double y_plus = dep_var.getValue();
			double x_plus = indep_vars[i].getValue();
			
			indep_vars[i].adjustValue(-2 * search_step);	// move indep var back 2 steps (one step behind where it started)
			runSim();
			double y_minus = dep_var.getValue();
			double x_minus = indep_vars[i].getValue();
			
			indep_vars[i].setValue(prev_position.axes[i]);	// set indep var back where it started
			
			gradient.axes[i] = (y_plus - y_minus) / (x_plus - x_minus);
		}
		
		StateVector dir_of_max = gradient.unit();
		for(int i = 0; i < indep_vars.size(); i++)
		{
			indep_vars[i].adjustValue(dir_of_max.axes[i] * indep_vars[i].walk_step);
		}
		runSim();
		
		if(verbose)
		{
			std::cout << "\niteration: " << iteration++ << std::endl;
			std::cout << "Gradient: " << std::endl;
			for(double& a : gradient.axes) std::cout << "  " << a << std::endl;
			std::cout << "Prev step: " << std::endl;
			for(double& a : dir_of_max.axes) std::cout << "  " << a << std::endl;
			blade->printInfo(true);
		}
		else
		{
			for(auto& a : indep_vars)
			{
				a.print(); std::cout << ",\t";
			}
			dep_var.print(); std::cout << std::endl;
		}
		if((dep_var.getValue() < dep_var.prev_val))
		{
			for(int i = 0; i < indep_vars.size(); i++)
			{
				indep_vars[i].setValue(prev_position.axes[i]);
			}
			runSim();
			std::cout << "\nCUTTING STEPS IN HALF" << std::endl;
			halves--;
			for(auto& a : indep_vars)
				if(!a.at_bounds) a.walk_step *= 0.5;
			search_step *= 0.5;
			std::cout << "new steps: walk: " << walk_step << ", search: " << search_step << std::endl;
		}
		else
		{
			for(auto& a : indep_vars)
				if(!a.at_bounds) a.walk_step *= 1.05;
			search_step *= 1.05;
		}
	}
	std::cout << "\nFinished! Final results:\n" << std::endl;
	blade->printInfo(true);
}

void BladeTester::attachBlade(Blade* blade)
{
	this->blade = blade;
}
void BladeTester::setDepVar(double* param, std::string name)
{
	dep_var = DepVar(param, name);
}
void BladeTester::addIndepVar(double* param, double lower, double upper, std::string name)
{
	indep_vars.push_back(IndepVar(param, lower, upper, name));
}
