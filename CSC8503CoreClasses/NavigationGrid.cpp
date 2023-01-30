#include "NavigationGrid.h"
#include "Assets.h"

#include <fstream>

using namespace NCL;
using namespace CSC8503;

const int LEFT_NODE		= 0;
const int RIGHT_NODE	= 1;
const int TOP_NODE		= 2;
const int BOTTOM_NODE	= 3;

const char WALL_NODE	= 'x';
const char FLOOR_NODE	= '.';

NavigationGrid::NavigationGrid()	{
	nodeSize	= 0;
	gridWidth	= 0;
	gridHeight	= 0;
	allNodes = std::vector<GridNode*>();
	position	= { 0.0f, 0.0f, 0.0f };
}

NavigationGrid::NavigationGrid(const std::string&filename, Vector3 pos) : NavigationGrid() {
	position = pos;

	std::ifstream infile(Assets::DATADIR + filename);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	allNodes = std::vector<GridNode*>();
	allNodes.reserve(gridWidth * gridHeight);

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			int i = (gridWidth * y) + x;
			GridNode* n = new GridNode();
			char type = 0;
			infile >> type;
			n->type = type;
			n->position = Vector3((float)(x * nodeSize), 0, (float)(y * nodeSize));
			allNodes.push_back(n);
		}
	}
	
	//now to build the connectivity between the nodes
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			GridNode* n = allNodes[(gridWidth * y) + x];		

			if (y > 0) { //get the above node
				n->connected[0] = allNodes[(gridWidth * (y - 1)) + x];
			}
			if (y < gridHeight - 1) { //get the below node
				n->connected[1] = allNodes[(gridWidth * (y + 1)) + x];
			}
			if (x > 0) { //get left node
				n->connected[2] = allNodes[(gridWidth * (y)) + (x - 1)];
			}
			if (x < gridWidth - 1) { //get right node
				n->connected[3] = allNodes[(gridWidth * (y)) + (x + 1)];
			}
			for (int i = 0; i < 4; ++i) {
				if (n->connected[i]) {
					if (n->connected[i]->type == '.' || n->connected[i]->type == 'U' || n->connected[i]->type == 'E' 
						|| n->connected[i]->type == 'r' || n->connected[i]->type == 'g' || n->connected[i]->type == 'b'  || n->connected[i]->type == 'c' ) {
						n->costs[i]		= 1;
					}
					if (n->connected[i]->type == 'x' || n->connected[i]->type == 'R' || n->connected[i]->type == 'G' || n->connected[i]->type == 'B') {
						n->connected[i] = nullptr; //actually a wall, disconnect!
					}
				}
			}
		}	
	}
}

NavigationGrid::~NavigationGrid()	
{
	for (GridNode* n : allNodes)
	{
		delete n;
	}
}

bool NavigationGrid::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
	Vector3 fpos = from - position;
	Vector3 tpos = to - position;

	//need to work out which node 'from' sits in, and 'to' sits in
	int fromX = ((int)(fpos).x / nodeSize);
	int fromZ = ((int)(fpos).z / nodeSize);

	int toX = ((int)(tpos).x / nodeSize);
	int toZ = ((int)(tpos).z / nodeSize);

	if (fromX < 0 || fromX > gridWidth - 1 ||
		fromZ < 0 || fromZ > gridHeight - 1) {
		return false; //outside of map region!
	}

	if (toX < 0 || toX > gridWidth - 1 ||
		toZ < 0 || toZ > gridHeight - 1) {
		return false; //outside of map region!
	}

	GridNode* startNode = allNodes[(fromZ * gridWidth) + fromX];
	GridNode* endNode	= allNodes[(toZ * gridWidth) + toX];

	std::vector<GridNode*>  openList;
	std::vector<GridNode*>  closedList;

	openList.push_back(startNode);

	startNode->f = 0;
	startNode->g = 0;
	startNode->parent = nullptr;

	GridNode* currentBestNode = nullptr;

	while (!openList.empty()) {
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == endNode) {			//we've found the path!
			GridNode* node = endNode;
			while (node != nullptr) {
				outPath.PushWaypoint(node->position);
				node = node->parent;
			}
			return true;
		}
		else {
			for (int i = 0; i < 4; ++i) {
				GridNode* neighbour = currentBestNode->connected[i];
				if (!neighbour) { //might not be connected...
					continue;
				}	
				bool inClosed	= NodeInList(neighbour, closedList);
				if (inClosed) {
					continue; //already discarded this neighbour...
				}

				float h = Heuristic(neighbour, endNode);				
				float g = currentBestNode->g + currentBestNode->costs[i];
				float f = h + g;

				bool inOpen		= NodeInList(neighbour, openList);

				if (!inOpen) { //first time we've seen this neighbour
					openList.emplace_back(neighbour);
				}
				if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
					neighbour->parent = currentBestNode;
					neighbour->f = f;
					neighbour->g = g;
				}
			}
			closedList.emplace_back(currentBestNode);
		}
	}
	return false; //open list emptied out with no path!
}

// Used by AI to patrol around a maze
bool NavigationGrid::PathToRandomNode(const Vector3& from, NavigationPath& outPath)
{
	int i;
	GridNode* n = allNodes[0];
	do
	{
		i = rand() % (gridWidth * gridHeight);
		n = allNodes[i];
		//std::cout << n->type << std::endl;
	} while (n->type != FLOOR_NODE);

	return FindPath(from, position + n->position, outPath);
}

GridNode* NavigationGrid::ClosestNode(Vector3 world_pos)
{
	float fromX = (world_pos - position).x / nodeSize;
	float fromZ = (world_pos - position).z / nodeSize;

	int x = floorf(fromX);
	int y = floorf(fromZ);

	return allNodes[(y * gridWidth) + x];
}

bool NavigationGrid::NodeInList(GridNode* n, std::vector<GridNode*>& list) const {
	std::vector<GridNode*>::iterator i = std::find(list.begin(), list.end(), n);
	return i == list.end() ? false : true;
}

GridNode*  NavigationGrid::RemoveBestNode(std::vector<GridNode*>& list) const {
	std::vector<GridNode*>::iterator bestI = list.begin();

	GridNode* bestNode = *list.begin();

	for (auto i = list.begin(); i != list.end(); ++i) {
		if ((*i)->f < bestNode->f) {
			bestNode	= (*i);
			bestI		= i;
		}
	}
	list.erase(bestI);

	return bestNode;
}

float NavigationGrid::Heuristic(GridNode* hNode, GridNode* endNode) const {
	return (hNode->position - endNode->position).Length();
}
