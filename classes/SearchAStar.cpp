/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

#include "SearchAStar.h"
#include <algorithm>

extern int gListSorted;


//manhattan heuristic function that calculates the path distance given the goal and the current position
int ManhattanDistance(unique_ptr<SNode>& current, unique_ptr<SNode>& goal)
{
	int distX = abs(current->x - goal->x);

	int distY = abs(current->y - goal->y);

	return  distX + distY;
}

bool CSearchAStar::NodeIsEqualTo(unique_ptr<SNode>& first, unique_ptr<SNode>& second)
{
	return first->x == second->x && first->y == second->y;
}

bool cmp(unique_ptr<SNode>& lhs, unique_ptr<SNode>& rhs)
{
	if (lhs != nullptr && rhs != nullptr)
		return lhs->score < rhs->score;
	else
		return false;
}

void CSearchAStar::SortListByCost(NodeList& list)
{
	sort(list.begin(), list.end(), cmp);
	gListSorted++;
}


SNode* CSearchAStar::FindIn(unique_ptr<SNode>& ptr, NodeList& list, NodeList& closed)
{
	bool found = false;
	//liner search
	for (auto& n : list)
	{
		found = n->x == ptr->x && n->y == ptr->y;
		if (found)
		{
			return n.get();
		}
	}
	for (auto& n : closed)
	{
		found = n->x == ptr->x && n->y == ptr->y;
		if (found)
		{
			return n.get();
		}
	}
	return nullptr;
}

int FindInPos(unique_ptr<SNode>& ptr, NodeList& closed)
{
	int i = 0;
	for (auto& n : closed)
	{
		if (n->x == ptr->x && n->y == ptr->y)
		{
			return i;
		}
		i++;
	}
	return -1;
}


void CSearchAStar::GetPathFromNode(unique_ptr<SNode>& node, NodeList& list)
{
	SNode* p = node->parent;
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

bool CSearchAStar::FindPath(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path)
{

	//1) Create OpenList an ClosedList
	NodeList OpenList;
	NodeList ClosedList;

	//2) Push the initial state(start) on to OpenList, set its cost to 0 , set its parent to 0 and calculate its score using the heuristic value

	unique_ptr<SNode> tmpS(new SNode);
	tmpS->x = start->x;
	tmpS->y = start->y;
	OpenList.push_back(move(tmpS));
	OpenList.front()->score = ManhattanDistance(OpenList.front(), goal);

	//	3) Until a goal state is found or OpenList is empty do:
	while (!OpenList.empty())
	{

		//Pop the first(best) element from OpenListand call it 'current'.
		unique_ptr<SNode> current(new SNode);
		current = move(OpenList.front());
		OpenList.pop_front();

		//If 'current' is the goal state, return success, construct the path and quit.
		if (NodeIsEqualTo(current, goal))
		{
			GetPathFromNode(current, path);

			unique_ptr<SNode> tmpG(new SNode);
			tmpG->x = goal->x;
			tmpG->y = goal->y;
			tmpG->score = OpenList.front()->score + 1;
			path.push_back(move(tmpG));

			return true;
		}

		//For each rule that can match 'current' do :
		//Apply the rule to generate a new state and call it 'n'

		for (int i = 0; i < 4; i++)
		{
			unique_ptr<SNode> next(new SNode);
			next->x = current->x;
			next->y = current->y;

			switch (i)
			{
			case 0:
				//north
				next->y--;
				break;
			case 1:
				//east
				next->x++;
				break;
			case 2:
				//south
				next->y++;
				break;
			case 3:
				//west
				next->x--;
				break;
			}

			//boundary check
			if (next->x < 0 || next->x >= terrain[0].size() || next->y < 0 || next->y >= terrain.size())
			{
				next.reset();
				//break;
			}
			else if (terrain[next->y][next->x] == Wall)
			{
				next.reset();
				//break;
			}
			else
			{

				//The cost of the new state is the cost of 'current' added to the cost of moving from 'current' to 'n':
				//newcost = current.cost + cost(current, n)
				int newcost = current->score + terrain[next->y][next->x];

				auto existingN = FindIn(next, ClosedList, OpenList);
				int posInClos = FindInPos(next, ClosedList);

				if (existingN)
				{
					//If 'n' is on OpenList or on ClosedList and newcost >= existing_n.cost then 
					if (newcost >= existingN->score)
						continue;

					//If 'n' is on OpenList or on ClosedList then let 'n' now refer to existing_n.
					next->parent = existingN->parent;
					next->score = existingN->score;
					next->x = existingN->x;
					next->y = existingN->y;
				}

				//Set the parent of 'n' to 'current'.Calculate the heuristic value.Set the score of 'n' to newcost + heuristic value.Set the cost of 'n' to newcost
				next->parent = current.get();
				next->score = newcost + ManhattanDistance(next, goal);

				//If 'n' was on ClosedList then remove it from ClosedList and push it onto OpenList.
				if (posInClos >= 0)
				{
					OpenList.push_front(move(OpenList[posInClos]));
					ClosedList.erase(ClosedList.begin() + posInClos);
				}

				if (!existingN)
				{
					//) If 'n' was not on OpenList or on ClosedList then push 'n' on to OpenList.
					OpenList.push_front(move(next));
				}
			}
		}
		ClosedList.push_front(move(current));
		//Reorder OpenList by cost(since either the cost of 'n' has been updated or 'n' has been pushed onto OpenList).
		SortListByCost(OpenList);
	}
	return false;
}
void CSearchAStar::CurrentPly(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path, unique_ptr<SNode>& current, NodeList& OpenList, NodeList& ClosedList)
{
	/*
	1) Create OpenListand ClosedList.
	2) Push the initial state(start) on to OpenList, set its parent to 0, set its cost to 0 and calculate its score using the heuristic value.
		3) Until a goal state is found or OpenList is empty do:
		(a)Remove(pop) the first element from OpenListand call it 'current'.
			(b)If OpenList was empty, return failure and quit.
			(c)If 'current' is the goal state, return success, construct the pathand quit
			(d) For each rule that can match 'current' do :
			i) Apply the rule to generate a new state and call it 'n'
			ii) The cost of the new state is the cost of 'current' added to the cost of moving from 'current' to 'n':
			newcost = current.cost + cost(current, n)
				iii) If 'n' is on OpenList or on ClosedList and newcost >= existing_n.cost then skip the rest of the loop and go back to(d)
				iv) If 'n' is on OpenList or on ClosedList then let 'n' now refer to existing_n.
				v) Set the parent of 'n' to 'current'.Calculate the heuristic value.Set the score of 'n' to newcost + heuristic value.Set the cost of 'n' to newcost.
				vi) If 'n' was on ClosedList then remove it from ClosedList and push it onto OpenList.
				vii) If 'n' was not on OpenList or on ClosedList then push 'n' on to OpenList.
				viii) Reorder OpenList by score(since either the cost of 'n' has been updated or 'n' has been pushed onto OpenList).
				e) Push 'current' on to ClosedList.
				*/


				//2) Push the initial state(start) on to OpenList, set its cost to 0 , set its parent to 0 and calculate its score using the heuristic value
	if (OpenList.empty())
	{
		unique_ptr<SNode> tmpS(new SNode);
		tmpS->x = start->x;
		tmpS->y = start->y;
		tmpS->parent = 0;
		tmpS->score = 0;
		OpenList.push_back(move(tmpS));
		OpenList.front()->score = ManhattanDistance(OpenList.front(), goal);
	}

	//	3) Until the OpenList is empty do:
	if (!OpenList.empty())
	{
		//If 'current' is the goal state, return success, construct the path and quit.
		if (NodeIsEqualTo(OpenList.front(), goal))
		{
			unique_ptr<SNode> tmpG(new SNode);
			tmpG->x = goal->x;
			tmpG->y = goal->y;
			tmpG->score = OpenList.front()->score + 1;
			path.push_back(move(tmpG));
			GetPathFromNode(move(OpenList.front()), path);
		}
		else
		{

			//Pop the first(best) element from OpenList and call it 'current'.
			current.reset();
			current = move(OpenList.front());
			OpenList.pop_front();

			//For each rule that can match 'current' do :
			//Apply the rule to generate a new state and call it 'n'

			for (int i = 0; i < 4; i++)
			{
				unique_ptr<SNode> next(new SNode);
				next->x = current->x;
				next->y = current->y;

				switch (i)
				{
				case 0:
					//north
					next->y--;
					break;
				case 1:
					//east
					next->x++;
					break;
				case 2:
					//south
					next->y++;
					break;
				case 3:
					//west
					next->x--;
					break;
				}

				//boundary check
				if (next->x < 0 || next->x >= terrain[0].size() || next->y < 0 || next->y >= terrain.size())
				{
					next.reset();
					//break;
				}
				else if (terrain[next->y][next->x] == Wall)
				{
					next.reset();
					//break;
				}
				else
				{
					//The cost of the new state is the cost of 'current' added to the cost of moving from 'current' to 'n':
					//newcost = current.cost + cost(current, n)
					int newcost = current->score + terrain[next->y][next->x];

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
					//Calculate the heuristic value. Set the score of 'n' to newcost + heuristic value. Set the cost of 'n' to newcost
					next->score = newcost + ManhattanDistance(next, goal);

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
		return;
	}
}
