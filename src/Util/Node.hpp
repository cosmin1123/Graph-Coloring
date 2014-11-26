#ifndef NODE_H
#define NODE_H
#include <vector>

class Node
{
private:
	unsigned int Id;
	unsigned int color;
	std::vector<Node*> neighbours;
public:
	Node(){Id = -1; color = -1; }
	Node(unsigned int assignId):Id(assignId){ color = 0; }
	unsigned int GetId() {return Id;}
	void SetId(unsigned int id) { Id = id; }
	unsigned int GetColor(){ return color; }
	void SetColor(const unsigned int c) { color = c;}
	std::vector<Node*>& GetNeighbours(){ return neighbours; }
	void SetNeighbours(const std::vector<Node*> & neigh) { neighbours = neigh; }
	bool operator==(const Node &n) { return this->Id == n.Id; }
	bool operator!=(const Node &n) { return !(*this == n);}
	bool operator<(const Node &n) { return this->Id < n.Id; }
	bool operator<=(const Node &n) { return this->Id <= n.Id; }
	bool operator>(const Node &n) { return this->Id > n.Id; }
	bool operator>=(const Node &n) { return this->Id >= n.Id; }

	inline bool IsNeighbour(unsigned int otherId)
	{
		std::vector<Node*>::iterator it = neighbours.begin();
		for (; it != neighbours.end(); it++)
		{
			if ((*it)->GetId() == otherId)
			{
				return true;
			}
		}
		return false;
	}
};

#endif