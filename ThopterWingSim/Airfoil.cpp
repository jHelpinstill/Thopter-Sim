#include "Airfoil.h"

Airfoil::DataPoint Airfoil::interp(double alpha) // assumes all alphas are bounded -180 to 180
{
	if(data.size() < 2)
	{
		std::cout << "Error: cannot interpolate with only one data point!" << std::endl;
		exit(0);
	}
	
	int mid;
	int lower = 0;
	int upper = data.size() - 1;
	while(lower < upper)
	{
		mid = (lower + upper) / 2;
		double candidate = data[mid].alpha;
		if(candidate < alpha)
		{
			lower = mid + 1;
		}
		else
			upper = mid - 1;
	}
	
	double alpha0 = data[mid - 1].alpha;
	double alpha1 = data[mid].alpha;
	
	double t = 0;
	if(alpha0 != alpha1)
		t = (alpha - alpha0) / (alpha1 - alpha0);
	
	DataPoint dp;
	dp.alpha = alpha;
	dp.cl = (1 - t) * data[mid - 1].cl + t * data[mid].cl;
	dp.cd = (1 - t) * data[mid - 1].cd + t * data[mid].cd;
	
	return dp;
}

double Airfoil::getCl(double alpha)
{
	return interp(alpha).cl;
}
double Airfoil::getCd(double alpha)
{
	return interp(alpha).cd;
}

bool Airfoil::attachData(std::string file_name, bool print)
{
	std::ifstream file(file_name);
	if(!file.is_open())
		return false;
	
	std::string line;
	while(!file.eof())
	{
		
		std::getline(file, line);
		std::stringstream ss(line);
		
		DataPoint dp;
		ss >> dp.alpha;
		ss >> dp.cl;
		ss >> dp.cd;

		if(data.size() && dp.alpha <= data[data.size() - 1].alpha)
		{
			break;
		}
		data.push_back(dp);
	}
	
	if(print)
	{
		std::cout << "Data collected: " << std::endl;
		printData();
	}
	return true;
}

void Airfoil::printData()
{
	std::cout << "alpha\tcl\tcd" << std::endl;
	for(int i = 0; i < data.size(); i++)
	{
		std::cout << data[i].alpha << '\t' << data[i].cl << '\t' << data[i].cd << std::endl;
	}
}
