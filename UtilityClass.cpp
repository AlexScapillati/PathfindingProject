/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

#include <iostream>
#include <fstream>
#include <windows.h>
#include <direct.h>
#include "UtilityClass.h"

string GetCurrentDir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	_getcwd(buff, FILENAME_MAX);
	string current_working_dir(buff);
	return current_working_dir + "\\";
}

bool OpenMap(TerrainMap& myMap, char c)
{
	vector<vector<int>> map;

	fstream fs;

	string MapFile;

	MapFile = GetCurrentDir() + "Maps\\" + c + "Map.txt";
	fs.open(MapFile);

	if (fs.is_open())
	{

		//get the map dimentions
		int mapX, mapY;

		fs >> mapX;
		fs >> mapY;


		for (int i = 0; i < mapY; i++)
		{
			//temporary variable for the row
			char tmp[20];
			//populate the temp arrat
			for (int j = 0; j < mapX; j++)
			{
				fs >> tmp[j];
			}
			vector<int> vtmp;
			//put the variables of the array in the vector
			for (int j = 0; j <= mapX; j++)
			{
				//when is not enter
				if (tmp[j] != '\n')
					vtmp.push_back(tmp[j] - 48);
			}
			map.push_back(vtmp);
		}

		fs.close();
		//put the temporary vector in the terrainmap vectors

		vector<ETerrainCost> et;

		for (int i = 0; i < mapY; i++)
		{
			for (int j = 0; j < mapX; j++)
			{
				ETerrainCost tmp;
				switch (map[i][j])
				{
				case 0:
					tmp = Wall;
					break;
				case 1:
					tmp = Clear;
					break;
				case 2:
					tmp = Wood;
					break;
				case 3:
					tmp = Water;
					break;
				}
				et.push_back(tmp);
			}
			myMap.push_back(et);
			et.clear();
		}

		// reverse the y
		reverse(myMap.begin(), myMap.end());

		return true;
	}
	else return false;

}

//Open the coord file
bool OpenCoord(unique_ptr<SNode>& start, unique_ptr<SNode>& goal, char c)
{
	fstream fs;

	string MapFile = GetCurrentDir() + "Maps\\" + c + "Coords.txt";

	fs.open(MapFile);

	int coords[4];

	if (fs.is_open())
	{
		for (int i = 0; i < 4; i++)
		{
			fs >> coords[i];
		}
		start->x = coords[0];
		start->y = coords[1];
		goal->x = coords[2];
		goal->y = coords[3];
		fs.close();
		return true;
	}
	else return false;
}


int FindNodelInMap(float currX, float currY, vector<IModel*>& vector)
{
	for (int i = 0; i < vector.size(); i++)
	{
		if (vector[i]->GetX() == currX && vector[i]->GetY() == currY) return i;
	}
	return -1;
}


bool NodeIsEqualTo(unique_ptr<SNode>& first, unique_ptr<SNode>& second)
{
	return first.get()->x == second.get()->x && first.get()->y == second.get()->y;
}

//overload that just outputs the path,the start and the goal
bool WriteToFile(NodeList& path, unique_ptr<SNode>& start, unique_ptr<SNode>& goal)
{
	ofstream fs;
	fs.open("output.txt");
	if (fs.is_open())
	{
		fs << "start: " << start->x << " " << start->y << "\n";
		fs << "goal: " << goal->x << " " << goal->y << "\n";

		fs << "Path: " << "\n";
		for (auto& it : path)
		{
			fs << it->x << " " << it->y << " " << it->score <<"\n";
		}
		fs << "List Sorted: " << to_string(gListSorted) << " Times \n";
		fs.close();
		return true;
	}
}

//simple factorial function
int factorial(int n)
{
	if (n > 1)
		return n * factorial(n - 1);
	else
		return 1;
}

//function that returns the binomial factor 
float binomial(int n, int i)
{
	float a = factorial(n);
	float b = factorial(i);
	float c = factorial(n - i);
	float d = b * c;

	return a / d;
}


//calculate the points with incrementing t and create the models
void Bezier(vector<IModel*>& controlPoints, deque<IModel*>& splineModels, int numberPoints, IMesh* mesh, string skin, float squareheight)
{
	//reverse(controlPoints.begin(), controlPoints.end());

	//calculate the incremental of t with the number of points a curve should have
	const float incremental = float(1.0f / (numberPoints + 1));
	float t = incremental;

	for (int i = 0; i < numberPoints; i++)
	{
		float finalX = 0.0f;
		float finalZ = 0.0f;

		//get the point for that t value 

		//for every control point
		const int n = controlPoints.size();
		for (int j = 0; j < n; j++)
		{
			//get the positions of that control point 
			const float x = controlPoints[j]->GetX();
			const float z = controlPoints[j]->GetZ();

			//calculate the value of the pernstein polynomial
			const float tmp = binomial(n - 1, j) * powf(t, j) * powf((1.0f - t), (n - 1 - j));

			//add the value temp to the final sum
			finalX += tmp * x;
			finalZ += tmp * z;
		}

		//create the model
		splineModels.push_back(mesh->CreateModel(finalX, squareheight, finalZ));
		splineModels.back()->SetSkin(skin);
		//increment the t
		t += incremental;
	}

	//remove control points
	//ResetListModels(controlPoints, mesh);
}

void ResetListModels(vector<IModel*>& v, IMesh* mesh)
{
	for (auto& it : v)
	{
		mesh->RemoveModel(it);
	}
	v.clear();
}

