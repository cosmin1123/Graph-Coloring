#include <random>
#include <iostream>
#include <chrono>
#include "Graph.hpp"
#include "Utils.hpp"
#include "omp.h"

bool Graph::Generate(unsigned int count
				  , unsigned int fillPercentage)
{
	if (fillPercentage > 100)
	{
		fillPercentage = 100;
	}
	if (count < 2)
	{
		return false;
	}
	try
	{
		allNodes.resize(count);
	}
	catch (...)
	{
		std::cerr << "Exception: Couldn't allocate that many nodes.\n";
		return 0;
	}

	unsigned int i = 0, edgesPerNode = ((count - 1) * fillPercentage) / 100;
	//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	unsigned seed = 0;

	std::mt19937 generator(seed);
	std::uniform_int_distribution<int> distribution(0, count - 1);

	std::vector<Node>::iterator currentNode = allNodes.begin();
	for (; currentNode != allNodes.end(); currentNode++)
	{
		currentNode->SetId(i++);
		currentNode->SetColor(0);
	}
	currentNode = allNodes.begin();
	for (; currentNode != allNodes.end(); currentNode++)
	{
		unsigned int settledNeighbours = currentNode->GetNeighbours().size();
		if (settledNeighbours > edgesPerNode)
		{
			continue;
		}
		unsigned int diff = edgesPerNode - settledNeighbours;
		for (i = 0; i < diff; i++)
		{
			unsigned int newNeighId;
			do
			{
				newNeighId = distribution(generator);
			}while (newNeighId == currentNode->GetId()
					|| currentNode->IsNeighbour(newNeighId));

			allNodes[newNeighId].GetNeighbours().push_back(&(*currentNode));
			currentNode->GetNeighbours().push_back(&allNodes[newNeighId]);
		}
	}
}

bool Graph::ColorSerial(unsigned int colorCount)
{
	std::vector<unsigned int> IdSet;
	for (int i = 0; i < allNodes.size(); i++)
	{
		IdSet.push_back(i);
	}
	while (IdSet.size())
	{
		std::vector<Node*> localMaxNodes;
		for(int i = 0; i < allNodes.size(); i++)
		{
			if (!Utils::IsInVector(IdSet, allNodes[i].GetId()))
			{
				continue;
			}
			bool isMax = true;
			for (int j = 0; j < allNodes[i].GetNeighbours().size(); j++)
			{
				if(allNodes[i].GetId() < allNodes[i].GetNeighbours()[j]->GetId()
					&& Utils::IsInVector(IdSet, allNodes[i].GetNeighbours()[j]->GetId()))
				{
					isMax = false;
					break;
				}
			}
			if (isMax)
			{
				localMaxNodes.push_back(&allNodes[i]);
			}
		}
		for(int j = 0; j < localMaxNodes.size(); j++)
		{
			bool colors[allNodes.size() + 1], maxColor;
			colors[0] = true;
			for(int i = 1; i < allNodes.size() + 1; i++)
			{
				colors[i] = false;
			}
			for (int i = 0; i < localMaxNodes[j]->GetNeighbours().size(); i++)
			{
				colors[localMaxNodes[j]->GetNeighbours()[i]->GetColor()] = true;
			}
			for (int i = 0; i < allNodes.size() + 1; i++)
			{
				if (!colors[i])
				{
					maxColor = i;
					break;
				}
			}
			if (maxColor > colorCount)
			{
				for (int i = 0; i < allNodes.size(); i++)
				{
					allNodes[i].SetColor(0);
				}
				return false;
			}
			localMaxNodes[j]->SetColor(maxColor);
			for (int i = 0; i < IdSet.size(); i++)
			{
				if(IdSet[i] == localMaxNodes[j]->GetId())
				{
					IdSet.erase(IdSet.begin() + i);
					break;
				}
			}
		}
	}
	return true;
}

bool Graph::TestColorCorrectness(unsigned int colorCount)
{
	for (int i = 0; i < allNodes.size(); i++)
	{
		if (allNodes[i].GetColor() > colorCount || allNodes[i].GetColor() == 0)
		{
			return false;
		}
		for (int j = 0; j < allNodes[i].GetNeighbours().size(); j++)
		{
			if (allNodes[i].GetColor() == allNodes[i].GetNeighbours()[j]->GetColor())
			{
				return false;
			}
		}
	}
	return true;
}

void Graph::ClearColors()
{
	for (int i = 0; i < allNodes.size(); i++)
	{
		allNodes[i].SetColor(0);
	}
}

void Graph::Print()
{
	for (int i = 0; i < allNodes.size(); i++)
	{
		std::cout << allNodes[i].GetId() << "(" << allNodes[i].GetColor() << "): ";
		int neighSize = allNodes[i].GetNeighbours().size();
		for (int j = 0; j < neighSize; j++)
		{
			std::cout << allNodes[i].GetNeighbours()[j]->GetId() << " ";
		}
		std::cout << std::endl;
	}
}
