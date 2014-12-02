#include "Graph.hpp"
#include <iostream>

#include <omp.h>

double time_diff() {
	static double begin_time = 0;
	// do something
	double diff = (omp_get_wtime() - begin_time) ; 

	if(begin_time != 0) {
		begin_time = 0;
	} else {
		begin_time = omp_get_wtime();;
	}
	return diff;
}

int main()
{
	Graph g;
	g.Generate(2000, 10);
	time_diff();
	g.ColorSerial(1000);
	std::cout << time_diff() << "\n";
	std::cout << "Correctness: " << g.TestColorCorrectness(1000) << "\n";

	g.ClearColors();

	time_diff();
	g.ColorOMP(1000);
	std::cout << time_diff() << "\n";


	std::cout << "Correctness: "  << g.TestColorCorrectness(1000) << "\n";
	

	//g.Print();
}