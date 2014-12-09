#include <iostream>
#include <fstream>
#include <cstdio>
#include <omp.h>
#include "../Core/Graph.hpp"
#include "../Core/Utils.hpp"

#define GSIZE 8

double time_diff() {
	static double begin_time = 0;

	double diff = (omp_get_wtime() - begin_time) ; 
	if(begin_time != 0) {
		begin_time = 0;
	} else {
		begin_time = omp_get_wtime();
	}
	return diff;
}

int main()
{
	double measuredTime;
	time_diff();
	int nthreads, tid;
    int i;

	Graph g;
    g.Generate(GSIZE, 50);
	std::vector<unsigned int> IdSet;

	std::cout << "Graf initial:\n";
	g.Print();

	for (int i = 0; i < g.allNodes.size(); i++)
	{
		IdSet.push_back(i);
	}

	while (IdSet.size())
	{
		std::vector<Node*> localMaxNodes;
		
		#pragma omp parallel for 
		for(int i = 0; i < g.allNodes.size(); i++)
		{
			if (!Utils::IsInVector(IdSet, g.allNodes[i].GetId()))
			{
				continue;
			}
			bool isMax = true;
			for (int j = 0; j < g.allNodes[i].GetNeighbours().size(); j++)
			{
				if(g.allNodes[i].GetId() < g.allNodes[i].GetNeighbours()[j]->GetId()
					&& Utils::IsInVector(IdSet, g.allNodes[i].GetNeighbours()[j]->GetId()))
				{
					isMax = false;
					break;
				}
			}
			if (isMax)
			{
				#pragma omp critical 
				localMaxNodes.push_back(&g.allNodes[i]);
			}
		}

		#pragma omp parallel for
		for(int j = 0; j < localMaxNodes.size(); j++)
		{
			unsigned int colors[g.allNodes.size()+1];
			for(int i = 0; i < g.allNodes.size()+1; i++)
			{
				colors[i] = 0;
			}
			colors[0] = 1;
			for (int i = 0; i < localMaxNodes[j]->GetNeighbours().size(); i++)
			{
				colors[localMaxNodes[j]->GetNeighbours()[i]->GetColor()] = 1;
			}

			for (int i = 0; i < g.allNodes.size()+1; i++)
			{
				if (colors[i] == 0)
				{
					#pragma omp critical
					localMaxNodes[j]->SetColor(i);
					break;
				}
			}
			
			for (int i = 0; i < IdSet.size(); i++)
			{
				if(IdSet[i] == localMaxNodes[j]->GetId())
				{
					#pragma omp critical
					IdSet.erase(IdSet.begin() + i);
					break;
				}
			}
		}
	}
	
	std::cout << "Graf final:\n";
	g.Print();

	measuredTime = time_diff();
	std::ofstream f("Timp OMP.txt", std::ios::app);
	f << GSIZE << ": " << measuredTime << "sec";
	f.close();
	return 0;
}