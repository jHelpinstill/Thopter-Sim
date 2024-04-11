#include "BladeTester.h"

void GradientAscent::runSim()
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
void GradientAscent::runTest(int num_rebounds, bool start_midpoint, bool verbose)
{
	double search_step = 0.05; // step size when adjusting dependent variables
	double walk_step = 0.2;
	if(start_midpoint) for(auto& var : indep_vars)
	{	// set independent variables to their allowed midpoint
		var.setValue((var.upper + var.lower) / 2);
	}
	runSim();
	
	StateVector gradient(indep_vars.size());
	StateVector prev_position(indep_vars.size());
	int iteration = 0;
	int halves = num_rebounds;
	double prev_dep_val;
	while(halves >= 0)
	{
		prev_dep_val = dep_var.getValue();
		
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
		
		// Move state vector position in the direction of the gradient
		StateVector dir_of_max = gradient.unit() * walk_step;
		for(int i = 0; i < indep_vars.size(); i++)
		{
			indep_vars[i].setValue(indep_vars[i].getValue()  + dir_of_max.axes[i]);
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
		for(auto& a : indep_vars)
		{
			a.print(); std::cout << ",\t";
		}
		dep_var.print(); std::cout << std::endl;
		std::cout << "Gradient: ";
		for(double& a : gradient.axes) std::cout << "  " << a;
		std::cout << std::endl;
		
		// if the output is now lower than it was, back up and reduce the walk step size
		if((dep_var.getValue() < prev_dep_val))
		{
			for(int i = 0; i < indep_vars.size(); i++)
			{
				indep_vars[i].setValue(prev_position.axes[i]);
			}
			runSim();
			std::cout << "\nCUTTING STEPS IN HALF" << std::endl;
			halves--;
			walk_step *= 0.5;
			search_step *= 0.5;
			std::cout << "new steps: walk: " << walk_step << ", search: " << search_step << std::endl;
		}
		else
		{
			walk_step *= 1.05;
			search_step *= 1.05;
		}
		results.push_back(ResultData(indep_vars, dep_var.getValue()));
	}
	std::cout << "\nFinished! Final results:\n" << std::endl;
	blade->printInfo(true);
	printResultsCSV();
}

void GradientAscent::attachBlade(Blade* blade)
{
	this->blade = blade;
}
void GradientAscent::setDepVar(double* param, std::string name)
{
	dep_var = Variable(param, name);
}
void GradientAscent::addIndepVar(double* param, double lower, double upper, std::string name)
{
	indep_vars.push_back(Variable(param, lower, upper, name));
}
void GradientAscent::printResultsCSV()
{
	for(auto& a : indep_vars)
		std::cout << a.name << "\t";
	std::cout << dep_var.name << std::endl;
	for(auto& r : results)
	{
		for(double v : r.indep_vars)
			std::cout << v << "\t";
		std::cout << r.dep_var << std::endl;
	}
}
