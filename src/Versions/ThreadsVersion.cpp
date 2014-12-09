#include <iostream>
#include <random>
#include <iostream>
#include <chrono>
#include <pthread.h>
#include "../Core/Graph.hpp"
#include "../Core/Utils.hpp"
using namespace std;

#define GSIZE 64

pthread_mutex_t allNodes_mutex;
pthread_mutex_t idSet_mutex;
pthread_barrier_t   barrier;

bool stillAlive = true;

struct ThreadParam{
	int startIndex;
	int numOfNodes;
	std::vector<unsigned int>* IdSet;
	std::vector<Node*>* localMaxNodes;
	std::vector<Node>* allNodes;

	ThreadParam(int index, int num, 
		std::vector<unsigned int>* set,
		std::vector<Node*>* max,
		std::vector<Node>* all)
	: startIndex(index), numOfNodes(num),IdSet(set), localMaxNodes(max),allNodes(all) {};
};

void* doThreadWork(void* argument);

int main(int argc, char** argv){
	if (argc  < 2){
		cout << "Usage: " << argv[0] << " <num_of_threads>" << endl;
		return -1;
	}

	int numOfThreads = atoi(argv[1]);

	Graph g;
    g.Generate(GSIZE, 50);

    std::cout << "Graf initial:\n";
	g.Print();

	std::vector<unsigned int> IdSet;
	for (int i = 0; i < g.allNodes.size(); i++)
	{
		IdSet.push_back(i);
	}

	pthread_t threads[numOfThreads];
	int numOfNodesPerThread = g.allNodes.size() / numOfThreads;
	int totalNodes = 0;
	pthread_barrier_init (&barrier, NULL, numOfThreads);

	std::vector<Node*> localMaxNodes;

	int ret;
	for(int i = 0; i < numOfThreads; i++)
	{
		int chunkSize;
		int startIndex = totalNodes;
		if (i == numOfThreads - 1){
			chunkSize = numOfNodesPerThread + (g.allNodes.size() - totalNodes - numOfNodesPerThread); 
		}
		else{
			chunkSize = numOfNodesPerThread;
		}
		totalNodes += numOfNodesPerThread;

		ThreadParam* param = new ThreadParam(startIndex,chunkSize,&IdSet,&localMaxNodes,&g.allNodes);

		ret = pthread_create( &threads[i], NULL, doThreadWork, (void*) param);
		if(ret)
	    {
	    	fprintf(stderr,"Error - pthread_create() return code: %d\n",ret);
	        exit(EXIT_FAILURE);
	    }
	}

	for (int i = 0; i < numOfThreads; i++)
	{
		pthread_join(threads[i], NULL);
	}

	std::cout << "Graf final:\n";
	g.Print();

}

void* doThreadWork(void* argument){
	ThreadParam* param = (ThreadParam*) argument;
	
	int startIndex =  param->startIndex;
	int numOfNodes = param->numOfNodes;
	std::cout << "I am doing " << startIndex << " to " << startIndex + numOfNodes << std::endl;
	std::vector<unsigned int>* IdSet = param->IdSet;
	// std::vector<Node*>* localMaxNodes = param->localMaxNodes;
	std::vector<Node>* allNodes = param->allNodes;
	
	while (IdSet->size())
	{
		std::vector<Node*> localMaxNodes;
		for(int i = startIndex; i < startIndex + numOfNodes; i++)
		{
			if (!Utils::IsInVector((*IdSet), (*allNodes)[i].GetId()))
			{
				continue;
			}
			bool isMax = true;
			for (int j = 0; j < (*allNodes)[i].GetNeighbours().size(); j++)
			{
				if((*allNodes)[i].GetId() < (*allNodes)[i].GetNeighbours()[j]->GetId()
					&& Utils::IsInVector((*IdSet), (*allNodes)[i].GetNeighbours()[j]->GetId()))
				{
					isMax = false;
					break;
				}
			}
			if (isMax)
			{
				localMaxNodes.push_back(&(*allNodes)[i]);
			}
		}

		for(int j = 0; j < localMaxNodes.size(); j++)
		{
			unsigned int colors[allNodes->size()+1];
			for(int i = 0; i < allNodes->size()+1; i++)
			{
				colors[i] = 0;
			}
			colors[0] = 1;
			for (int i = 0; i < localMaxNodes[j]->GetNeighbours().size(); i++)
			{
				colors[localMaxNodes[j]->GetNeighbours()[i]->GetColor()] = 1;
			}

			for (int i = 0; i < allNodes->size()+1; i++)
			{
				if (colors[i] == 0)
				{
					pthread_mutex_lock(&allNodes_mutex);
					localMaxNodes[j]->SetColor(i);
					pthread_mutex_unlock(&allNodes_mutex);
					break;
				}
			}

			for (int i = 0; i < IdSet->size(); i++)
			{
				if((*IdSet)[i] == localMaxNodes[j]->GetId())
				{
					pthread_mutex_lock(&idSet_mutex);
					IdSet->erase(IdSet->begin() + i);
					pthread_mutex_unlock(&idSet_mutex);
					break;
				}
			}
		}

	}
}

