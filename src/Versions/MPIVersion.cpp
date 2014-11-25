#include <iostream>
#include <fstream>
#include <omp.h>
#include "mpi.h"
#include "../Core/Graph.hpp"
#include "../Core/Utils.hpp"
#include <cstdio>

#define GSIZE	416

struct NeighInfo
{
	unsigned int Id;
	unsigned int color;
};

struct CNod
{
	unsigned int Id;
	unsigned int color;
	unsigned int neighbourCount;
	NeighInfo* neighbours;
	CNod(){neighbours = NULL;}
	std::vector<unsigned int> IdSet;
};

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

int main(int argc, char** argv)
{
	double measuredTime;

	int rank, size;

    MPI_Status status;
    MPI_Request request, request2;
    
    // Init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(!rank)
    {
    	time_diff();
    }

    //------- Data Initialization and Distribution -------//
    bool ImDone = false, active = true;
    std::vector<unsigned int> activeThreads;
    for (int i = 0; i < size; i++)
    {
    	activeThreads.push_back(i);
    }
    unsigned int nodesCount = GSIZE / size;
    if (rank == size - 1)
    {
    	nodesCount += (GSIZE % size);
    }
    std::vector<CNod> myNodes;
    Graph g;
    g.Generate(GSIZE, 50);			//Generates the same for all threads with a static seed

    MPI_Barrier(MPI_COMM_WORLD);

    if (size == 1)
    {
    	//std::cout << "Graf initial:\n";
    	//g.Print();
    	std::cout << "Graf final:\n";
    	g.ColorSerial(g.allNodes.size());
    	//g.Print();
    	return 0;
    }
    
    int startingPoint = rank * (GSIZE / size);
    int endPoint = rank * (GSIZE / size) + nodesCount;
    for (int i = startingPoint; i < endPoint; i++)
    {
    	CNod nod;
    	nod.Id = i;
    	nod.color = 0;
    	nod.neighbourCount = g.allNodes[i].GetNeighbours().size();
    	nod.neighbours = new NeighInfo[nod.neighbourCount];
    	for (int j = 0; j < nod.neighbourCount; j++)
    	{
    		unsigned int neighId = g.allNodes[i].GetNeighbours()[j]->GetId();
    		nod.neighbours[j].Id = neighId;
    		nod.neighbours[j].color = 0;
    		nod.IdSet.push_back(neighId);
    	}
    	myNodes.push_back(nod);
    }
    std::vector<unsigned int> mySet;
    for(int i = 0; i < myNodes.size(); i++)
    {
    	mySet.push_back(myNodes[i].Id);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //------- Algorithm computation -------//
    while(true)
    {
    	// Checking life signals
    	if (rank == 0)
    	{
    		unsigned int finishedThreads = (active == true) ? 0 : 1;
    		for (int i = 1; i < size; i++)
    		{
    			unsigned int ls;
    			MPI_Recv(&ls, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
    			if (ls == 0)
    			{
    				finishedThreads++;
    			}
    		}
    		unsigned int die = (finishedThreads == size) ? 1 : 0;
    		for (int i = 1; i < size; i++)
    		{
    			MPI_Send(&die, 1, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD);
    		}
    		if (die == 1)
    		{
    			break;
    		}
    	}
    	else
    	{
    		unsigned int ls = (active == true) ? 1 : 0;
    		MPI_Send(&ls, 1, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);
    		unsigned int die;
    		MPI_Recv(&die, 1, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD, &status);
    		if (die == 1)
    		{
    			break;
    		}
    	}

    	std::vector<NeighInfo> toBeSent;
    	if (active)
    	{
			// Finding all the max value nodes that the thread owns
			std::vector<CNod*> localMaxima;
			for (int i = 0; i < myNodes.size(); i++)
			{
				if (!Utils::IsInVector(mySet, myNodes[i].Id))
				{
					continue;
				}
				bool isMax = true;
				for (int j = 0; j < myNodes[i].neighbourCount; j++)
				{
					if (myNodes[i].neighbours[j].Id > myNodes[i].Id
						&& Utils::IsInVector(myNodes[i].IdSet, myNodes[i].neighbours[j].Id)
						)
					{
						isMax = false;
						break;
					}
				}
				if (isMax)
				{
					localMaxima.push_back(&myNodes[i]);
				}
			}
			// For each of the maximal value nodes found
			for (int i = 0; i < localMaxima.size(); i++)
			{
				// Determining the minimal color & applying it
				bool colors[g.allNodes.size()+1];
				for (int j = 1; j <= g.allNodes.size(); j++)
				{
					colors[j] = false;
				}
				colors[0] = true;
				for (int j = 0; j < localMaxima[i]->neighbourCount; j++)
				{
					colors[localMaxima[i]->neighbours[j].color] = true; 
				}
				for (int j = 0; j <= g.allNodes.size(); j++)
				{
					if (!colors[j])
					{
						localMaxima[i]->color = j;
						break;
					}
				}
				// Updating colors on local level
				for (int j = 0; j < myNodes.size(); j++)
				{
					if (myNodes[j].Id != localMaxima[i]->Id)
					{
						for(int k = 0; k < myNodes[j].neighbourCount; k++)
						{
							if (myNodes[j].neighbours[k].Id == localMaxima[i]->Id)
							{
								myNodes[j].neighbours[k].color = localMaxima[i]->color;
							}
						}
					}
				}
				// Building the message all the others will receive
				NeighInfo msg;
				msg.Id = localMaxima[i]->Id;
				msg.color = localMaxima[i]->color;
				toBeSent.push_back(msg);
				// Removing the colored nodes from the personal IdSet
	    		for (int j = 0; j < mySet.size(); j++)
	    		{
	    			if (mySet[j] == localMaxima[i]->Id)
	    			{
	    				mySet.erase(mySet.begin() + j);
	    				break;
	    			}
	    		}
	    		for (int j = 0; j < myNodes.size(); j++)
	    		{
	    			if(myNodes[j].Id != localMaxima[i]->Id)
	    			{
	    				for(int k = 0; k < myNodes[j].IdSet.size(); k++)
	    				{
	    					if(myNodes[j].IdSet[k] == localMaxima[i]->Id)
	    					{
	    						myNodes[j].IdSet.erase(myNodes[j].IdSet.begin() + k);
	    						break;
	    					}
	    				}
	    			}
	    		}
			}
			localMaxima.clear();
		}

		MPI_Barrier(MPI_COMM_WORLD);
    	//----------- Syncronization process
		// Sending and receiving the updates
		std::vector<NeighInfo> updates;
		for (int i = 0; i < size; i++)
		{
			if (active)
			{
				if (rank == i)
				{
					if (ImDone)
					{
						unsigned int finishVal = -1;
						for (int j = 0; j < size; j++)
						{
							if (activeThreads[j] != rank && activeThreads[j] != -1)
							{
								MPI_Send(&finishVal, 1, MPI_UNSIGNED, j, 1, MPI_COMM_WORLD);
							}
							else if (activeThreads[j] == rank)
							{
								activeThreads[j] == -1;
							}
						}
						active = false;
					}
					else
					{
						if (!mySet.size())
						{
							ImDone = true;
						}
						for (int k = 0; k < size; k++)
						{
							if (activeThreads[k] != rank && activeThreads[k] != -1)
							{
								unsigned int updatesToBeSent = toBeSent.size();
								MPI_Send(&updatesToBeSent, 1, 
									MPI_UNSIGNED, k, 1, MPI_COMM_WORLD);
								for (int j = 0; j < updatesToBeSent; j++)
								{
									unsigned int currentId = toBeSent[j].Id;
									unsigned int currentColor = toBeSent[j].color;
									MPI_Send(&currentId, 1, 
										MPI_UNSIGNED, k, 1, MPI_COMM_WORLD);
									MPI_Send(&currentColor, 1, 
										MPI_UNSIGNED, k, 1, MPI_COMM_WORLD);
								}
							}
						}
						toBeSent.clear();
					}
				}
				else
				{
					if (activeThreads[i] != -1)
					{
						unsigned int updatesToReceive;
						MPI_Recv(&updatesToReceive, 1, MPI_UNSIGNED, 
							i, 1, MPI_COMM_WORLD, &status);
						if (updatesToReceive == -1)
						{
							activeThreads[i] = -1;
						}
						else
						{
							for (int j = 0; j < updatesToReceive; j++)
							{
								unsigned int recv_id, recv_col;
								MPI_Recv(&recv_id, 1, MPI_UNSIGNED, 
									i, 1, MPI_COMM_WORLD, &status);
								MPI_Recv(&recv_col, 1, MPI_UNSIGNED, 
									i, 1, MPI_COMM_WORLD, &status);
								NeighInfo update;
								update.Id = recv_id;
								update.color = recv_col;
								updates.push_back(update);
							}
						}
					}
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
		if (active)
		{
			// Applying all the relevant updates on local nodes
			for (int i = 0; i < updates.size(); i++)
			{
				for (int j = 0; j < myNodes.size(); j++)
				{
					for (int k = 0; k < myNodes[j].neighbourCount; k++)
					{
						if (myNodes[j].neighbours[k].Id == updates[i].Id)
						{
							myNodes[j].neighbours[k].color = updates[i].color;
							for (int ind = 0; ind < myNodes[j].IdSet.size(); ind++)
							{
								if (myNodes[j].IdSet[ind] == updates[i].Id)
								{
									myNodes[j].IdSet.erase(myNodes[j].IdSet.begin() + ind);
									break;
								}
							}
						}
					}
				}
			}
			updates.clear();
		}
		MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    //------- Data Gathering -------//
    
    if (!rank)
    {
    	unsigned int otherNodesCount = g.allNodes.size() - myNodes.size();
    	for (int i = 0; i < otherNodesCount; i++)
    	{
    		NeighInfo msg;
    		MPI_Recv(&msg, 2, MPI_UNSIGNED, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
    		//MPI_Recv(&col, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
    		g.allNodes[msg.Id].SetColor(msg.color);
    	}
    	for (int i = 0; i < myNodes.size(); i++)
    	{
    		g.allNodes[myNodes[i].Id].SetColor(myNodes[i].color);
    	}
    	//g.Print();
    }
    else
    {
    	for(int i = 0; i < myNodes.size(); i++)
    	{
    		NeighInfo msg;
    		msg.Id = myNodes[i].Id;
    		msg.color = myNodes[i].color;
    		MPI_Send(&msg, 2, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);
    		//MPI_Send(&col, 1, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);
    	}
    }
	
    MPI_Barrier(MPI_COMM_WORLD);

    if (!rank)
    {
    	measuredTime = time_diff();
    	std::ofstream f("Timp MPI.txt", std::ios::app);
    	f << GSIZE << ": " << measuredTime << "sec";
    	f.close();
    }

    MPI_Finalize();	
	return 0;
}