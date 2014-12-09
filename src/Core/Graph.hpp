#include <vector>
#include "Node.hpp"

class Graph
{

public:
	Graph(){}
	std::vector<Node> allNodes;
	bool Generate(unsigned int count
				, unsigned int fillPercentage);
	bool ColorSerial(unsigned int colorCount);
	bool TestColorCorrectness(unsigned int colorCount);
	void ClearColors();
	void Print();
};