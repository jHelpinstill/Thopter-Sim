#include "BladeTester.h"

void PlotGenerator::runSim()
{
	blade->reset();
	blade->rebuild();
	for(auto& v : dep_vars)
		v.reset();
	
	double period = 1 / blade->freq;
	double dt = period / iters_per_period;
	int iterations = iters_per_period * sim_num_periods;
	for(int i = 0; i < iterations; i++)
	{
		blade->update(1.225, dt);
		for(auto& v : dep_vars)
			if(v.average) v.recordAvg(dt, blade->t);
	}
}
void PlotGenerator::runRealTime(double periods_to_skip_over, int sampling_freq)
{
	blade->reset();
	blade->rebuild();
	for(auto& v : dep_vars)
		v.reset();
	
	double period = 1 / blade->freq;
	double dt = period / iters_per_period;
	int iterations = iters_per_period * sim_num_periods;
	int skipped_iters = (iters_per_period * periods_to_skip_over);
	
	std::vector<Variable> dummy_indep;
	for(int i = 0; i < iterations; i++)
	{
		blade->update(1.225, dt);
		for(auto& v : dep_vars)
			if(v.average) v.recordAvg(dt, blade->t);
		if(i >= skipped_iters)
			if(!((i - skipped_iters) % sampling_freq))
				results.push_back(ResultDatum(dummy_indep, dep_vars));
	}
	std::cout << "\nFinished! Final results:\n" << std::endl;
	printResultsCSV();
}
void PlotGenerator::run(bool verbose)
{
	indep_var.setValue(from);
	int divisions = (to - from) / incr;
	for(int i = 0; i <= divisions; i++)
	{
		runSim();
		if(verbose)
		{
			blade->printInfo(true);
		}
		results.push_back(ResultDatum(indep_var.getValue(), dep_vars));
		indep_var.adjustValue(incr);
	}
	std::cout << "\nFinished! Final results:\n" << std::endl;
	printResultsCSV();
}
void PlotGenerator::run(double from, double to, double incr, bool verbose)
{
	setAttributes(from, to, incr);
	run(verbose);
}
void PlotGenerator::setAttributes(double from, double to, double incr)
{
	this->from = from;
	this->to = to;
	this->incr = incr;
}
void PlotGenerator::attachBlade(Blade* blade)
{
	this->blade = blade;
}
void PlotGenerator::setIndepVar(double* param, std::string name)
{
	indep_var = Variable(param, name);
}
void PlotGenerator::addDepVar(double* param, std::string name, double output_scale, bool average)
{
	dep_vars.push_back(Variable(param, name, output_scale, average));
}
void PlotGenerator::addDepVar(double* param, double* param2, std::string name, double output_scale, bool average, bool post_divide)
{
	dep_vars.push_back(Variable(param, param2, name, output_scale, average, post_divide));
}
void PlotGenerator::printResultsCSV()
{
	std::cout << indep_var.name;
	if(indep_var.name.length() != 0) std::cout << ", ";
	for(auto& v : dep_vars)
		std::cout << v.name << ", ";
	std::cout << "\b\b \b" << std::endl;
	for(auto& r : results)
	{
		for(auto& v : r.indep_vars)
			std::cout << v << ", ";
		for(auto& v : r.dep_vars)
			std::cout << v << ", ";
		std::cout << "\b\b \b" << std::endl;
	}
}
