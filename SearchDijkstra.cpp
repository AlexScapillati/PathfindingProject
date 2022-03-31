/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

#include "SearchDijkstra.h"
#include <algorithm>

extern int gListSorted;

bool CSearchDijkstra::compareNode(unique_ptr<SNode>& first, unique_ptr<SNode>& second)
{
	return first.get()->x == second.get()->x && first.get()->y == second.get()->y;
}

bool CSearchDijkstra::CompareNodeCost(unique_ptr<SNode>& lhs, unique_ptr<SNode>& rhs)
{
	return lhs.get()->score < rhs.get()->score;
}

void CSearchDijkstra::SortListByCost(NodeList& list)
{
	//using lambda (for some reason the function below is not working)
	sort(list.begin(), list.end(), [](const auto& lhs, const auto& rhs) { return lhs.get()->score < rhs.get()->score; });
	gListSorted++;
}

SNode* CSearchDijkstra::FindIn(unique_ptr<SNode>& ptr, NodeList& list, NodeList& closed)
{
	bool found = false;
	//liner search
	for (auto& n : list)
	{
		found = n.get()->x == ptr.get()->x && n.get()->y == ptr.get()->y;
		if (found)
		{
			return n.get();
			break;
		}
	}
	for (auto& n : closed)
	{
		found = n.get()->x == ptr.get()->x && n.get()->y == ptr.get()->y;
		if (found)
		{
			return n.get();
			break;
		}
	}
	return nullptr;
}

int CSearchDijkstra::FindInPos(unique_ptr<SNode>& ptr, NodeList& closed)
{
	int i = 0;
	for (auto& n : closed)
	{
		if (n.get()->x == ptr.get()->x && n.get()->y == ptr.get()->y)
		{
			return i;
		}
		i++;
	}
	return -1;
}

void getPathFromNode(unique_ptr<SNode>& node, NodeList& list)
{
	SNode* p = node.get()->parent;

	//until the parent is null
	while (p != nullptr)
	{
		//create a temp snode pointer and get the current's parent
		SNode* tmp = p->parent;
		//move the current to the path list 
		list.push_front(make_unique<SNode>(*p));
		// the parent is now the current
		p = tmp;
	}
}

bool CSearchDijkstra::FindPath(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path)
{
	/*
	1) Create OpenList
	2) Push the initial state(start) on to OpenList, set its cost to 0 and set its parent to 0.
		3) Until a goal state is found or OpenList is empty do:
		(a)Pop the first(best) element from OpenListand call it 'current'.
			(b)If OpenList was empty, return failure and quit.
			(c)If 'current' is the goal state, return success, construct the pathand quit.
			(d)For each rule that can match 'current' do :
			i) Apply the rule to generate a new stateand call it 'n'
			ii) The cost of the new state is the cost of 'current' added to the cost of moving from 'current' to 'n':
			newcost = current.cost + cost(current, n)
				iii) If 'n' is on OpenList and newcost >= existing_n.cost then skip the rest of the loop and go back to(d)
				iv) If 'n' is on OpenList then let 'n' now refer to existing_n.
				v) Set the parent of 'n' to 'current'.Set the cost of 'n' to newcost.
				vi) If 'n' was not on OpenList then push 'n' on to OpenList.
				vii) Reorder OpenList by cost(since either the cost of 'n' has been updated or 'n' has been pushed onto OpenList).
	*/

	//1) Create OpenList
	NodeList OpenList;
	NodeList ClosedList;

	//2) Push the initial state(start) on to OpenList, set its cost to 0 and set its parent to 0.

	unique_ptr<SNode> tmpS(new SNode);
	tmpS->x = start->x;
	tmpS->y = start->y;
	OpenList.push_back(move(tmpS));
	OpenList.front().get()->score = 0;


	//	3) Until a goal state is found or OpenList is empty do:

	while (!OpenList.empty())
	{
		//Pop the first(best) element from OpenListand call it 'current'.
		unique_ptr<SNode> current(new SNode);
		current = move(OpenList.front());
		OpenList.pop_front();

		//If 'current' is the goal state, return success, construct the path and quit.
		if (compareNode(current, goal))
		{
			unique_ptr<SNode> tmpG(new SNode);
			tmpG->x = goal->x;
			tmpG->y = goal->y;
			tmpG->score = current->score + 1;
			path.push_back(move(tmpG));
			getPathFromNode(move(current), path);
			return true;
		}

		//For each rule that can match 'current' do :
		//Apply the rule to generate a new state and call it 'n'


		for (int i = 0; i < 4; i++)
		{
			unique_ptr<SNode> next(new SNode);
			next.get()->x = current.get()->x;
			next.get()->y = current.get()->y;

			switch (i)
			{
			case 0:
				//north
				next.get()->y--;
				break;
			case 1:
				//east
				next.get()->x++;
				break;
			case 2:
				//south
				next.get()->y++;
				break;
			case 3:
				//west
				next.get()->x--;
				break;
			}

			//boundary check
			if (next.get()->x < 0 || next.get()->x >= terrain[0].size() || next.get()->y < 0 || next.get()->y >= terrain.size())
			{
				next.reset();
				//break;
			}
			else if (terrain[next.get()->y][next.get()->x] == Wall)
			{
				next.reset();
				//break;
			}
			else
			{
				//The cost of the new state is the cost of 'current' added to the cost of moving from 'current' to 'n':
				//newcost = current.cost + cost(current, n)
				int newcost = current.get()->score + terrain[next.get()->y][next.get()->x];

				auto existingN = FindIn(next, ClosedList, OpenList);
				int posInClos = FindInPos(next, ClosedList);

				//If 'n' is on OpenList or on ClosedList
				if (existingN)
				{
					// and newcost >= existing_n.cost then skip the rest
					if (newcost >= existingN->score)
						continue;

					//If 'n' is on OpenList or on ClosedList then let 'n' now refer to existing_n.
					next->parent = existingN->parent;
					next->score = existingN->score;
					next->x = existingN->x;
					next->y = existingN->y;
				}

				//Set the parent of 'n' to 'current'.
				next->parent = current.get();
				//Calculate the heuristic value. Set the score of 'n' to newcost. Set the cost of 'n' to newcost
				next->score = newcost;

				//If 'n' was on ClosedList
				if (posInClos >= 0)
				{
					// then remove it from ClosedList and push it onto OpenList.
					OpenList.push_front(move(ClosedList[posInClos]));
					ClosedList.erase(ClosedList.begin() + posInClos);

				}
				//If 'n' was not on OpenList or on ClosedList
				if (!existingN)
				{
					//then push 'n' on to OpenList.
					OpenList.push_front(move(next));
				}
			}
		}
		ClosedList.push_front(move(current));
		SortListByCost(OpenList);

	}
	//the openlist is empty and no path is found
	return false;
}


void CSearchDijkstra::CurrentPly(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path, unique_ptr<SNode>& current, NodeList& OpenList, NodeList& ClosedList)
{
	//2) Push the initial state(start) on to OpenList, set its cost to 0 and set its parent to 0.
	if (OpenList.empty())
	{
		unique_ptr<SNode> tmpS(new SNode);
		tmpS->x = start->x;
		tmpS->y = start->y;
		tmpS->parent = 0;
		tmpS->score = 0;
		OpenList.push_back(move(tmpS));
		OpenList.front().get()->score = 0;
	}

	unique_ptr<SNode> next(new SNode);

	//If 'current' is the goal state, return success, construct the path and quit.
	if (compareNode(OpenList.front(), goal))
	{
		unique_ptr<SNode> tmpG(new SNode);
		tmpG->x = goal->x;
		tmpG->y = goal->y;
		tmpG->score = OpenList.front()->score + 1;
		path.push_back(move(tmpG));
		getPathFromNode(OpenList.front(), path);
	}
	else
	{
		//	3) Until a goal state is found or OpenList is empty do:

		if (!OpenList.empty())
		{
			//Pop the first(best) element from OpenListand call it 'current'.
			current = move(OpenList.front());
			OpenList.pop_front();

			//For each rule that can match 'current' do :
			//Apply the rule to generate a new state and call it 'n'
			for (int i = 0; i < 4; i++)
			{
				next.reset(new SNode);
				next.get()->x = current.get()->x;
				next.get()->y = current.get()->y;

				switch (i)
				{
				case 0:
					//north
					next.get()->y--;
					break;
				case 1:
					//east
					next.get()->x++;
					break;
				case 2:
					//south
					next.get()->y++;
					break;
				case 3:
					//west
					next.get()->x--;
					break;
				}

				//boundary check
				if (next.get()->x < 0 || next.get()->x >= terrain[0].size() || next.get()->y < 0 || next.get()->y >= terrain.size())
				{
					next.reset();
					//break;
				}
				else if (terrain[next.get()->y][next.get()->x] == Wall)
				{
					next.reset();
					//break;
				}
				else
				{
					//The cost of the new state is the cost of 'current' added to the cost of moving from 'current' to 'n':
					//newcost = current.cost + cost(current, n)
					int newcost = current.get()->score + terrain[next.get()->y][next.get()->x];

					auto existingN = FindIn(next, ClosedList, OpenList);
					int posInClos = FindInPos(next, ClosedList);

					//If 'n' is on OpenList or on ClosedList
					if (existingN)
					{
						// and newcost >= existing_n.cost then skip the rest
						if (newcost >= existingN->score)
							continue;

						//If 'n' is on OpenList or on ClosedList then let 'n' now refer to existing_n.
						next->parent = existingN->parent;
						next->score = existingN->score;
						next->x = existingN->x;
						next->y = existingN->y;
					}

					//Set the parent of 'n' to 'current'.
					next->parent = current.get();
					//Calculate the heuristic value. Set the score of 'n' to newcost. Set the cost of 'n' to newcost
					next->score = newcost;

					//If 'n' was on ClosedList
					if (posInClos >= 0)
					{
						// then remove it from ClosedList and push it onto OpenList.
						OpenList.push_front(move(ClosedList[posInClos]));
						ClosedList.erase(ClosedList.begin() + posInClos);

					}
					//If 'n' was not on OpenList or on ClosedList
					if (!existingN)
					{
						//then push 'n' on to OpenList.
						OpenList.push_front(move(next));
					}
				}
			}
			//Add 'current' to ClosedList
			ClosedList.push_front(move(current));
			//Reorder OpenList by cost(since either the cost of 'n' has been updated or 'n' has been pushed onto OpenList).
			SortListByCost(OpenList);
		}
	}

}

