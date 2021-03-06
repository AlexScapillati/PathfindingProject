/*-------------------
    Alex Scapillati
    Univerity of Central Lancashire
    07/02/2020
---------------------*/


// Definitions.h
// =============
//
// Type definitions to support path finding 
//

#pragma once
#include <vector>
#include <deque>
#include <memory>


using namespace std;

// Represents a node in the search tree.
struct SNode
{
  int x;            // x coordinate
  int y;            // y coordinate
  int score;        // used in more complex algorithms
  SNode* parent = 0; // note use of raw pointer here
};

// Lists of nodes (e.g Open, Closed and Paths) are implemented as double-ended queues
using NodeList = deque<unique_ptr<SNode>>;

// Terrain costs for map squares
enum ETerrainCost
{
  Clear = 1,
  Water = 3,
  Wood  = 2,
  Wall  = 0
};

// Maps of any size are implemented as 2D vectors
using TerrainMap = vector<vector<ETerrainCost>>;


struct SNodeReturn
{
    SNode* node;
    bool fromOpenlist;
    int pos;
};



class Definitions
{
};
