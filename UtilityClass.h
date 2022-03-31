
/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

#pragma once
#include "Definitions.h"
#include <string>
#include "TL-Engine.h"

using namespace tle;
using namespace std;

extern int gListSorted;

string GetCurrentDir();

bool OpenMap(TerrainMap& myMap, char c);

bool OpenCoord(unique_ptr<SNode>& start, unique_ptr<SNode>& goal, char c);

int FindNodelInMap(float currX, float currY, vector<IModel*>& vector);

bool NodeIsEqualTo(unique_ptr<SNode>& first, unique_ptr<SNode>& second);

int factorial(int n);

float binomial(int n, int i);

void BernsteinPolynomial(float t, float& xSum, float& ySum, vector<IModel*>& controlPoints);

void ResetListModels(vector<IModel*>& v, IMesh* mesh);

//calculate the points with incrementing t and create the models
void Bezier(vector<IModel*>& controlPoints, deque<IModel*>& splineModels, int numberPoints, IMesh* mesh, string skin, float squareheight);

bool WriteToFile(NodeList& path, unique_ptr<SNode>& start, unique_ptr<SNode>& goal);
