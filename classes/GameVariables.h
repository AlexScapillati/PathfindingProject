
/*-------------------
	Alex Scapillati
	Univerity of Central Lancashire
	07/02/2020
---------------------*/

#pragma once
#include "TL-Engine.h"
#include "Definitions.h"
#include <vector>

using namespace std;

//Game Constants

const float kSquareSide = 10.0f;			//the length of the square
const float kStartMapX = 0.0f;
const float kStartMapY = 0.0f;
const float kSquarePathHeight = 0.1f;
const float kSquareHeight = 0.01f;			//for avoiding the disappearing of the square
const float kPlayerY = 0.01f;				//since we are not modifying the player y it is a constant
const float kGameSpeed = 0.1f;
int tickPathGen = 1;					//how many game loops there is a path step
const float kScaleSquare = 2.5f;
const float kScaleFloor = 10.0f;
const float kPlayerScaling = 5.0f;
const int kNumberPoints = 50;			//number of points that the spline is composed
const int kTickIncremental = 10;

const int kWindowHeight = 900;
const int kWindowWidth = 900;

//camera
const ECameraType kCameraMode = kManual;
const float kCameraAngle = 90.0f;			//the camera angle so that it seems 2d


//UI
const float kDistanceTexts = 50.0f;
const float kTextPosX = 200.0f;
const float kTextPosY = 300.0f;

//enum for the game loop
enum EGameMode
{
	eMenu, ePlaying, eFinish
}eGame;

//Game Global variables

// Create a 3D engine (using TLX engine here) and open a window for it
I3DEngine* myEngine = New3DEngine(kTLX);


//no need to specify what each of these are
ICamera* myCamera;
IFont* myFont;
IMesh* mSquare;
IMesh* mPlayer;
IMesh* mGuard;
IModel* player;
IModel* guard;
IMesh* mPoint;
ISprite* menu;

//vectors for storing the square models
vector<IModel*> vControlPoints;
deque<IModel*> vSplinePoints;


enum ESquareState
{
	blank, open, closed
};

struct SMapSquare
{
	IModel* model;
	ESquareState state;			//if it is colored red or violet or is not still visited
	ETerrainCost cost;
};

vector<SMapSquare*> vNodes;


//Pathfinding algorithm setup
vector<ESearchType> vAlgorithms{ AStar,Dijkstra,BreadthFirst };
TerrainMap map;
NodeList path;
unique_ptr<SNode> start(new SNode);
unique_ptr<SNode> goal(new SNode);
ISearch* PathFinder;
unique_ptr<SNode> current;
NodeList openlist;
NodeList closedlist;
int pathGenTimer = tickPathGen;
bool pathDrawn = false;
bool canRun = false;
float timePathFound = 0.0f;
unique_ptr<SNode> prevStart(new SNode);
int gListSorted = 0;
bool bFound = false;

vector<string> vMaps
{ "Map m","Map d","Map c","Map n","Map s","Map t", "Map x" };
vector<string> vAlgorithmsTx
{ "A*","Dijkstra","BreathFirst" };
vector<char> vMapsFilesCh
{ 'm','d','c','n','s','t','x' };

//menu variables
const int kNumMaps = vMaps.size();
const int kNumAlgs = vAlgorithms.size();
int selectSearch = 0;
int selectMap = 0;
int selectionType = 0;
char mapCharacter;		//the character for loading the map

//Game Files 
//Colors
const string fViolet = "violet.png";
const string fRed = "red.png";
const string fBlack = "black.png";
const string fGreen = "green.png";
const string fBlue = "blue.png";
const string fWhite = "white.png";
const string fBrown = "brown.png";


//Meshes
const string fSquare = "Square.x";
const string fSierra = "sierra.x";
const string fCasual_A = "casual_A.x";

//Texts
const string fFont = "font1.bmp";

const string kSPathFoundIn[2] = { "Path Found in: ","s " };
const string kSPathGenSpeed = "Path Gen Speed: ";
const string kSSorted[2] = { " OpenList Sorted: "," Times " };
const string kSNoPath = "No path found";
const string kSPathFound = "Path found";

//Keys
const EKeyCode eKeyExit = Key_Escape;
const EKeyCode eKeyEnter = Key_Return;
const EKeyCode eKeySelectUp = Key_Up;
const EKeyCode eKeySelectDown = Key_Down;
const EKeyCode eKeyPressMouseL = Mouse_LButton;
const EKeyCode eKeyPressMouseR = Mouse_RButton;


