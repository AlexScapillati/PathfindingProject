/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

#include "SearchBreadthFirst.h" // Declaration of this class

bool  CSearchBreadthFirst::compareNode(unique_ptr<SNode>& first, unique_ptr<SNode>& second)
{
	return first.get()->x == second.get()->x && first.get()->y == second.get()->y;
}

// function that checks in the open list and in the closed list if the next node is already visited
//only for breadthfirst
bool  CSearchBreadthFirst::Visited(TerrainMap& terrain, NodeList& OpenList, NodeList& ClosedList, unique_ptr<SNode>& next)
{
	//boundary check
	if (next.get()->x >= terrain[0].size() || next.get()->x < 0)
	{
		return true;
	}
	else if (next.get()->y >= terrain.size() || next.get()->y < 0)
	{
		return true;
	}
	//if it is a wall is not avaiable
	else if (terrain[next.get()->y][next.get()->x] == Wall)
	{
		return true;
	}
	else
	{
		//for each element of the openlist 
		for (auto& node : OpenList)
		{
			if (compareNode(node, next))
			{
				return true;
			}
		}
		//for each element of the closed list 
		for (auto& node : ClosedList)
		{
			if (compareNode(node, next))
			{
				return true;
			}
		}
		return false;
	}

}

void CSearchBreadthFirst::GetPathFromNode(unique_ptr<SNode>& node, NodeList& list)
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

bool CSearchBreadthFirst::FindPath(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path)
{
	//Description of the algorythm
	/*

	1) Create OpenListand ClosedList
	2) Push the initial state(start) on to OpenList
		3) Until a goal state is found or OpenList is empty do:
		(a)Remove(pop) the first element from OpenListand call it 'current'.
			(b)If OpenList was empty, return failure and quit.
			(c)If 'current' is the goal state, return success and quit
			(d) For each rule that can match 'current' do :
			i) Apply the rule to generate a new state
			ii) If the new state has not already been visited, add the new state to end of OpenList.
			(e)Add 'current' to ClosedList
	*/


	//1) Create OpenListand ClosedList
	NodeList OpenList;
	NodeList ClosedList;

	// 2) Push the initial state(start) on to OpenList
	unique_ptr<SNode> tmpS(new SNode);
	tmpS->x = start->x;
	tmpS->y = start->y;
	tmpS->parent = 0;
	tmpS->score = 0;
	OpenList.push_back(move(tmpS));

	//3) Until a goal state is found or OpenList is empty do:

	bool found = false;
	bool empty = false;

	while (!found || !empty)
	{

		// (a)Remove(pop) the first element from OpenList and call it 'current'.

		unique_ptr<SNode> current(new SNode);
		current = move(OpenList.front());
		OpenList.pop_front();

		//	(c)If 'current' is the goal state, return success and quit

		if (compareNode(current, goal))
		{
			unique_ptr<SNode> tmpG(new SNode);
			tmpG->x = goal->x;
			tmpG->y = goal->y;
			tmpG->score = current->score + 1;
			path.push_back(move(tmpG));
			GetPathFromNode(move(current), path);
			return true;
		}

		//	(d) For each rule that can match 'current' do:

		//     i) Apply the rule to generate a new state

		//		generate north, east, south, west

		for (int i = 0; i < 4; i++)
		{
			unique_ptr<SNode> next(new SNode);
			next.get()->x = current.get()->x;
			next.get()->y = current.get()->y;
			next.get()->parent = current.get();

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



			//     ii) If the new state has not already been visited, add the new state to end of OpenList.
			if (!Visited(terrain, OpenList, ClosedList, next))
			{
				next.get()->score = terrain[next.get()->y][next.get()->x];
				OpenList.push_back(move(next));
			}
		}

		//	(b)If OpenList was empty, return failure and quit.

		if (OpenList.empty())
		{
			return false;
		}
		//  (e)Add 'current' to ClosedList

		ClosedList.push_front(move(current));

	}

}

void CSearchBreadthFirst::CurrentPly(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path, unique_ptr<SNode>& current, NodeList& OpenList, NodeList& ClosedList)
{
	//Description of the algorythm
	/*

	1) Create OpenListand ClosedList
	2) Push the initial state(start) on to OpenList
		3) Until a goal state is found or OpenList is empty do:
		(a)Remove(pop) the first element from OpenListand call it 'current'.
			(b)If OpenList was empty, return failure and quit.
			(c)If 'current' is the goal state, return success and quit
			(d) For each rule that can match 'current' do :
			i) Apply the rule to generate a new state
			ii) If the new state has not already been visited, add the new state to end of OpenList.
			(e)Add 'current' to ClosedList
	*/


	//1) Create OpenListand ClosedList

	// 2) Push the initial state(start) on to OpenList
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

	//3) Until a goal state is found or OpenList is empty do:
	if (!OpenList.empty())
	{
		// (a)Remove(pop) the first element from OpenList and call it 'current'.

		current = move(OpenList.front());
		OpenList.pop_front();

		//	(c)If 'current' is the goal state, return success and quit


			//	(d) For each rule that can match 'current' do:
			//     i) Apply the rule to generate a new state
			//		generate north, east, south, west
		for (int i = 0; i < 4; i++)
		{
			unique_ptr<SNode> next(new SNode);
			next.get()->x = current.get()->x;
			next.get()->y = current.get()->y;
			next.get()->parent = current.get();

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

			//     ii) If the new state has not already been visited, add the new state to end of OpenList.
			if (!Visited(terrain, OpenList, ClosedList, next))
			{
				next.get()->score = terrain[next.get()->y][next.get()->x];
				OpenList.push_back(move(next));
			}
		}

		//If 'current' is the goal state, return success, construct the path and quit
		if (compareNode(current, goal))
		{
			unique_ptr<SNode> tmpG(new SNode);
			tmpG->x = goal->x;
			tmpG->y = goal->y;
			tmpG->score = current->score + 1;
			path.push_back(move(tmpG));
			GetPathFromNode(current, path);
		}

		//Add 'current' to ClosedList
		ClosedList.push_front(move(current));
	}
}

