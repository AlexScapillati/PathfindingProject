/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

// Assignment.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace

#include "classes\Definitions.h"
#include "classes\UtilityClass.h"
#include "classes\Search.h"
#include "classes\SearchFactory.h"
#include "classes\GameVariables.h"

#include <vector>
using namespace tle;

//draw the path with little black dots
void DrawPath()
{
	
	for (unsigned int i = 0; i < path.size(); i++)
	{
		//get the coordinates of the node
		int currX = path[i]->x;
		int currY = path[i]->y;

		//get them into game world coordinates
		currX = kStartMapX + (currX * kSquareSide);
		currY = kStartMapY + (currY * kSquareSide);

		//create the model at that coordinates
		vControlPoints.push_back(mPoint->CreateModel(currX, kSquarePathHeight, currY));
		vControlPoints.back()->SetSkin(fBlack);
	}

	reverse(vControlPoints.begin(), vControlPoints.end());

	//draw spline
	Bezier(vControlPoints, vSplinePoints, kNumberPoints, mPoint, fBlack, kSquarePathHeight);
}

//create the basics for the menu to show
void SetupMenu()
{
	//create a menu
	myFont = myEngine->LoadFont(fFont);
	myCamera = myEngine->CreateCamera(kCameraMode);
}

//setup the map from the files
void MapSetup()
{
	//if the map is successfully opened 
	if (OpenMap(map, mapCharacter))
	{
		//get the starting point
		float posX = kStartMapX;
		float posZ = kStartMapY;

		//go through each element of the map and push the model in the corresponding vector while creating them 
		for (const auto& z : map)
		{
			for (const auto x : z)
			{
				SMapSquare* tmp = new SMapSquare;
				switch (x)
				{
				case Clear:
					//create model
					tmp->model = mSquare->CreateModel(posX, kSquareHeight, posZ);
					//set skin
					tmp->model->SetSkin(fWhite);
					//scale it
					tmp->model->Scale(kScaleFloor);
					//set its state to 0
					tmp->state = blank;
					//and the cost
					tmp->cost = x;
					//push it to the temp vector
					vNodes.push_back(tmp);
					break;
				case Wall:
					tmp->model = mSquare->CreateModel(posX, kSquareHeight, posZ);
					tmp->model->SetSkin(fBlack);
					tmp->model->Scale(kScaleFloor);
					tmp->state = blank;
					tmp->cost = x;
					vNodes.push_back(tmp);
					break;
				case Wood:
					tmp->model = mSquare->CreateModel(posX, kSquareHeight, posZ);
					tmp->model->SetSkin(fGreen);
					tmp->model->Scale(kScaleFloor);
					tmp->state = blank;
					tmp->cost = x;
					vNodes.push_back(tmp);
					break;
				case Water:
					tmp->model = mSquare->CreateModel(posX, kSquareHeight, posZ);
					tmp->model->SetSkin(fBlue);
					tmp->model->Scale(kScaleFloor);
					tmp->state = blank;
					tmp->cost = x;
					vNodes.push_back(tmp);
					break;
				}
				posX += kSquareSide;
			}
			posX = kStartMapX;
			posZ += kSquareSide;
		}

		//and the coords file too
		if (OpenCoord(start, goal, mapCharacter))
		{
			//put the start and goal on the map
			float playerPosX = kStartMapX + (start->x * kSquareSide);
			float playerPosZ = kStartMapY + (start->y * kSquareSide);

			player->SetPosition(playerPosX, 0.0f, playerPosZ);

			float goalPosX = kStartMapX + (goal->x * kSquareSide);
			float goalPosZ = kStartMapY + (goal->y * kSquareSide);

			guard->SetPosition(goalPosX, 0.0f, goalPosZ);
		}
	}
}

//setup and load every game component and setup the path
void GameSetup()
{
	//start setup 

	// Game Models
	mSquare = myEngine->LoadMesh(fSquare);
	mPoint = myEngine->LoadMesh(fSquare);

	//player
	mPlayer = myEngine->LoadMesh(fSierra);
	player = mPlayer->CreateModel();
	player->Scale(kPlayerScaling);
	//guard
	mGuard = myEngine->LoadMesh(fCasual_A);
	guard = mGuard->CreateModel();
	guard->Scale(kPlayerScaling);


	//setup the map on the world
	MapSetup();

	//camera setup based on the map size
	int columns = map[0].size();
	int rows = map.size();

	//set the y based on the biggest dimension
	myCamera->SetY(((columns > rows ? columns : rows)* kSquareSide));
	myCamera->SetZ((rows * kSquareSide) / 2 + kStartMapX - kSquareSide / 2);
	myCamera->SetX((columns * kSquareSide) / 2 + kStartMapY - kSquareSide / 2);
	myCamera->RotateX(kCameraAngle);
	//End setup
}

//"Menu" that allows to select the path algorithm and the map
void SelectMenu()
{
	//if we are selecting the maps or the algorithms 
	switch (selectionType)
	{
	case 0:
		//select by key
		if (myEngine->KeyHit(eKeySelectUp))
		{
			if (selectSearch > 0)
			{
				selectSearch--;
			}
		}
		else if (myEngine->KeyHit(eKeySelectDown))
		{
			if (selectSearch < kNumAlgs - 1)
			{
				selectSearch++;
			}
		}

		//draw the algorithms
		for (int i = 0; i < vAlgorithms.size(); i++)
		{
			//if the search value is now display it red
			if (selectSearch == i)
			{
				myFont->Draw(vAlgorithmsTx[i], kTextPosX, kTextPosY + (kDistanceTexts * i), kRed);
			}
			else
			{
				myFont->Draw(vAlgorithmsTx[i], kTextPosX, kTextPosY + (kDistanceTexts * i), kBlue);
			}
		}

		//if enter is pressed
		if (myEngine->KeyHit(eKeyEnter))
		{
			//go to select map
			selectionType = true;

			//instantiate the pathfinding class
			PathFinder = NewSearch(vAlgorithms[selectSearch]);
		}
		break;

	case 1:
		//select by key
		if (myEngine->KeyHit(eKeySelectUp))
		{
			if (selectMap > 0)
			{
				selectMap--;
			}
		}
		else if (myEngine->KeyHit(eKeySelectDown))
		{
			if (selectMap < kNumMaps - 1)
			{
				selectMap++;
			}
		}

		//draw the maps
		for (int i = 0; i < vMaps.size(); i++)
		{
			if (selectMap == i)
			{
				myFont->Draw(vMaps[i], kTextPosX, kTextPosY + (kDistanceTexts * i), kRed);
			}
			else
			{
				myFont->Draw(vMaps[i], kTextPosX, kTextPosY + (kDistanceTexts * i), kBlue);
			}
		}

		//if enter is pressed
		if (myEngine->KeyHit(eKeyEnter))
		{
			//get the character associated with the selection 
			mapCharacter = vMapsFilesCh[selectMap];

			//change the game type
			eGame = ePlaying;

			timePathFound = myEngine->Timer();

			//setup the game
			GameSetup();
		}
		break;
	}


	//if it is in this window you can close the game
	if (myEngine->KeyHit(eKeyExit))
	{
		myEngine->Stop();
	}
}

//Display the list on screen
void DisplayList(NodeList& list)
{
	//draw the list 
	for (auto& it : list)
	{
		//get the coordinates of the node
		int currX = it->x;
		int currY = it->y;

		int pos = currY * map[0].size() + currX;
		switch (vNodes[pos]->state)
		{
		case blank:
			vNodes[pos]->state = open;
			vNodes[pos]->model->SetSkin(fViolet);
			break;
		case open:
			vNodes[pos]->state = closed;
			vNodes[pos]->model->SetSkin(fRed);
			break;
		case closed:
			break;
		}
	}
}

//it resetst the map squares to the original map
void ResetMap(NodeList& list)
{
	//draw the list 
	for (auto& it : list)
	{
		//get the coordinates of the node
		int currX = it->x;
		int currY = it->y;

		int pos = currY * map[0].size() + currX;
		vNodes[pos]->state = blank;
		switch (vNodes[pos]->cost)
		{
		case Clear:
			vNodes[pos]->model->SetSkin(fWhite);
			break;
		case Wood:
			vNodes[pos]->model->SetSkin(fGreen);
			break;
		case Water:
			vNodes[pos]->model->SetSkin(fBlue);
			break;
		}
	}
}

//function that take the mouse position relative to the window and get the position relative to the map to get a new start point
//it returns true if the new node is different to the previous
bool TakeMousePos()
{
	//get mouse positions
	int x = myEngine->GetMouseX();
	int y = myEngine->GetMouseY();

	int sizeX = map[0].size();
	int sizeY = map.size();

	//if the map dimensions are different
	if (sizeX < sizeY)
	{
		sizeX = sizeY + (sizeX - sizeY);
	}
	else
	{
		sizeY = sizeX + (sizeX - sizeY);
	}

	//transform the coordinates of the mouse to the map coordinates
	x /= kWindowWidth / sizeX;
	y /= kWindowWidth / sizeY;

	//y axis is flipped
	int tmp = sizeX - 1;
	y = abs(y - tmp);

	//get the 1d position
	int pos = (y * sizeX) + x;

	//boundary check
	if (y < map.size() && x < map[0].size())
	{
		//check if the position is not a wall
		if (vNodes[pos]->cost != Wall)
		{
			//set the new start node
			start.reset(new SNode);
			start->x = x;
			start->y = y;

			//if the new start is not equal to the previous
			if (!NodeIsEqualTo(start, prevStart))
			{
				//get the coordinates in world space for moving the player
				x = kStartMapX + (x * kSquareSide);
				y = kStartMapY + (y * kSquareSide);

				//move player
				player->SetPosition(x, kPlayerY, y);
				player->LookAt(guard);
				player->Scale(kPlayerScaling);

				//copy the start node to the prevstart
				prevStart->x = start->x;
				prevStart->y = start->y;

				return true;
			}
			else return false;
		}
	}
}

bool FindPathInstant()
{
	//find path
	if (PathFinder->FindPath(map, start, goal, path))
	{
		WriteToFile(path, start, goal);
		//draw the path
		DrawPath();
		//take the timer
		timePathFound = myEngine->Timer();
		return true;
	}
	else
	{
		return false;
	}
}

void FindPathPly()
{
	//delete the open list
	ResetMap(openlist);

	//the pathfinding algorithm takes a step
	PathFinder->CurrentPly(map, start, goal, path, current, openlist, closedlist);

	//if the openlist is empty then there is not a possible path to be taken
	if (openlist.empty())
	{
		canRun = false;
		bFound = false;
	}
	else
	{
		//display the lists
		DisplayList(openlist);
		DisplayList(closedlist);
		//write to file
		WriteToFile(path, start, goal);
	}
}

//reset everything
void ClearMap()
{
	ResetListModels(vControlPoints, mPoint);
	for (auto& it : vSplinePoints)
	{
		mPoint->RemoveModel(it);
	}
	vSplinePoints.clear();
	ResetMap(closedlist);
	ResetMap(openlist);
	openlist.clear();
	closedlist.clear();
	path.clear();
	gListSorted = 0;
}

//the actual gameplay
void GameUpdate()
{
	if (canRun)
	{
		//display path in real time
		//decrease the timer
		pathGenTimer--;

		//if the timer is 0
		if (pathGenTimer < 0)
		{
			if (path.empty())
			{
				//take a step
				FindPathPly();
				//reset timer
				pathGenTimer = tickPathGen;
			}
			else
			{
				//draw path 
				DrawPath();
				//reset just the path
				path.clear();
				//take the time taken
				timePathFound = myEngine->Timer();
				canRun = false;
				bFound = true;
			}
		}
	}
	else
	{
		//instant path gen 
		if (myEngine->KeyHeld(eKeyPressMouseL))
		{
			//if the node clicked is not the preavious
			if (TakeMousePos())
			{
				ClearMap();
				if (FindPathInstant())
				{
					bFound = true;
				}
			}
		}
		//real time path gen
		else if (myEngine->KeyHit(eKeyPressMouseR))
		{
			if (TakeMousePos())
			{
				ClearMap();
				canRun = true;
			}
		}
	}

	//increase/decreease the path generation delay
	float wheelMov = myEngine->GetMouseWheelMovement();
	if (wheelMov > 0.0f)
	{
		tickPathGen += kTickIncremental;
	}
	if (wheelMov < 0.0f)
	{
		if (tickPathGen > kTickIncremental)
			tickPathGen -= kTickIncremental;
	}


	//Output variables in the title of the window 
	string pathFoundIn = kSPathFoundIn[0] + to_string(timePathFound) + kSPathFoundIn[1];
	string pathGenSpeed = kSPathGenSpeed + to_string(tickPathGen);
	string stringSorted = kSSorted[0] + to_string(gListSorted) + kSSorted[1];

	//display on the title screen the time that the pathfinding algorith took
	myEngine->SetWindowCaption(pathFoundIn + pathGenSpeed + stringSorted + (bFound ? kSPathFound : kSNoPath));



	if (myEngine->KeyHeld(Key_Up))
	{
		myCamera->MoveY(0.1f);
	}
	else if (myEngine->KeyHeld(Key_Down))
	{
		myCamera->MoveY(-0.1f);
	}


	//go back
	if (myEngine->KeyHit(eKeyExit))
	{

		ClearMap();

		//reset the map
		for (auto& it : vNodes)
		{
			mSquare->RemoveModel(it->model);
		}
		vNodes.clear();

		//reset the model that are not usually resetted
		mGuard->RemoveModel(guard);
		mPlayer->RemoveModel(player);
		myCamera->SetPosition(0.0f, 0.0f, 0.0f);
		myCamera->ResetOrientation();
		map.clear();
		delete(PathFinder);
		//reset the menu variables
		selectSearch = 0;
		selectMap = 0;
		selectionType = 0;
		eGame = eMenu;
	}
}

void main()
{
	//start the engine 
	myEngine->StartWindowed(kWindowWidth, kWindowHeight);

	auto media = GetCurrentDir() + "\Media";
	
	// Add default folder for meshes and other media
	myEngine->AddMediaFolder(media);

	/**** Set up your scene here ****/

	//setup menu
	eGame = eMenu;
	SetupMenu();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		switch (eGame)
		{
		case eMenu:
			SelectMenu();
			break;
		case ePlaying:
			GameUpdate();
			break;
		case eFinish:
			break;
		}
	}

	// Delete the 3D engine now we are finished with it
	if (myEngine->IsActive())
		myEngine->Delete();
}