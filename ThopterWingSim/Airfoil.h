#ifndef AIRFOIL_
#define AIRFOIL_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Airfoil
{
private:
	struct DataPoint
	{
		double alpha;
		double cl;
		double cd;
		DataPoint() : alpha(0), cl(0), cd(0){}
	};
	
	std::vector<DataPoint> data;
	
	DataPoint interp(double alpha);

public:
	Airfoil(){}
	Airfoil(std::string name) : name(name) {}
	
	std::string name = "";
	
	double getCl(double alpha);
	double getCd(double alpha);
	
	bool attachData(std::string file_name, bool print = false);
	void printData();
};

#endif
