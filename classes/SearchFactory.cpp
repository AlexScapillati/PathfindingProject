/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

// SearchFactory.cpp
// =================
//
// Factory function implementation to create CSearchXXX objects 
//

#include "SearchBreadthFirst.h"
#include "SearchDijkstra.h"
#include "SearchAStar.h"

#include "SearchFactory.h" // Factory declarations

// Create new search object of the given type and return a pointer to it.
// Note the returned pointer type is the base class. This is how we implement polymorphism.
ISearch* NewSearch(ESearchType search)
{
	switch (search)
	{
	case BreadthFirst:
		return new CSearchBreadthFirst();

	case Dijkstra:
		return new CSearchDijkstra();

	case AStar:
		return new CSearchAStar();
	}
}

class SearchFactory
{
};
