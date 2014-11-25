#include "Graph.hpp"
#include <iostream>

int main()
{
	Graph g;
	g.Generate(1000, 100);
	g.ColorSerial(1000);
	std::cout << g.TestColorCorrectness(1000);
	//g.Print();
}