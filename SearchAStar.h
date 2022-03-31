/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

#pragma once

#include "Definitions.h"  // Type definitions
#include "Search.h"       // Base (=interface) class definition

// NoStar search class definition

// Inherit from interface and provide implementation for 0* algorithm
class CSearchAStar : public ISearch
{
	// I have not implemented any constructors or destructors.
	// Whether you need some is up to how you choose to do your implementation.

	// Constructs the path from start to goal for the given terrain
	bool FindPath(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path);

	void CurrentPly(TerrainMap& terrain, unique_ptr<SNode>& start, unique_ptr<SNode>& goal, NodeList& path, unique_ptr<SNode>& current, NodeList& openlist, NodeList& closedlist);

	bool NodeIsEqualTo(unique_ptr<SNode>& first, unique_ptr<SNode>& second);

	void SortListByCost(NodeList& list);
	
	void GetPathFromNode(unique_ptr<SNode>& node, NodeList& list);

	SNode* FindIn(unique_ptr<SNode>& ptr, NodeList& list, NodeList& closed);

};
