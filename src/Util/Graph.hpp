#include <vector>
#include "Node.hpp"

class Graph
{
private:
	std::vector<Node> allNodes;
public:
	Graph(){}
	bool Generate(unsigned int count
				, unsigned int fillPercentage);
	//bool ColorSerial(unsigned int colorCount);
	bool ColorMPI(unsigned int colorCount);
	bool ColorOMP(unsigned int colorCount);
	bool ColorThreads(unsigned int colorCount);
	bool TestColorCorrectness(unsigned int colorCount);
	void ClearColors();
	void Print();
	std::vector<Node> getAllNodes();

};