#include "Util/Graph.hpp"
#include "Versions/SerialSolution.hpp"
#include <iostream>

int main()
{
	Graph g;
	SerialSolution s;
	g.Generate(100, 100);
	s.ColorSerial(100, g.getAllNodes());
	std::cout << g.TestColorCorrectness(100);
	//g.Print();
}
