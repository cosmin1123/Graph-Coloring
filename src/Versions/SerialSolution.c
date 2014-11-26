#include "SerialSolution.hpp"
#include "../Util/Node.hpp"
#include "../Util/Utils.hpp"

bool SerialSolution::ColorSerial(unsigned int colorCount, std::vector<Node> allNodes)
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
			unsigned int colors[allNodes.size()], maxColor;
			for(int i = 0; i < allNodes.size()+1; i++)
			{
				colors[i] = 0;
			}
			for (int i = 0; i < localMaxNodes[j]->GetNeighbours().size(); i++)
			{
				colors[localMaxNodes[j]->GetNeighbours()[i]->GetColor()] = 1;
			}
			for (int i = 0; i < allNodes.size()+1; i++)
			{
				if (colors[i] == 0)
				{
					maxColor = i;
					break;
				}
			}
			if (maxColor >= colorCount)
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