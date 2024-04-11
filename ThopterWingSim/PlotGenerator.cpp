#include "BladeTester.h"

void PlotGenerator::runSim()
{
	blade->reset();
	blade->rebuild();
	
	double period = 1 / blade->freq;
	double dt = period / iters_per_period;
	int iterations = iters_per_period * sim_num_periods;
	for(int i = 0; i < iterations; i++)
	{
		blade->update(Vec2(0, 0), 1.225, dt);
	}
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
			std::cout << indep_var.name << ": " << indep_var.getValue() << "\t" << dep_var.name << ": " << dep_var.getValue() << std::endl;
			blade->printInfo(true);
		}
		results.push_back(ResultData(indep_var.getValue(), dep_var.getValue()));
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
void PlotGenerator::setDepVar(double* param, std::string name)
{
	dep_var = Variable(param, name);
}
void PlotGenerator::printResultsCSV()
{
	std::cout << indep_var.name << ", ";
	std::cout << dep_var.name << std::endl;
	for(auto& r : results)
	{
		std::cout << r.indep_var * indep_output_scale << ", " << r.dep_var * dep_output_scale << std::endl;
	}
}
