#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <time.h>
#include <tuple>
#include <chrono>

using namespace std;

bool debug = false;

void printPuzzle(vector<vector<int>> ship);

const int BUFFERWIDTH = 24;
const int BUFFERHIGHT = 9;
const int SHIPHIGHT = 9;
const int SHIPWIDTH = 12;
const int PORTALTIME = 4;


struct node {
	vector<vector<int>> ship;
	vector<vector<int>> buffer;
	int row;
	int col;
	vector<vector<int>> prev;
	int numToLoad;
	int cost;
	int zone;	//0 == ship, 1 == buffer, 2 == truck
	int heur;
	vector<vector<vector<int>>> path;
};

class puzzles {
private:
	//Storage
	unsigned long expandedNodeCount;
	unsigned int maxQueueSize;

	//node tracking
	vector<node> frontier;
	vector<node> closed;

	//Heuristics
	int heuristic(node &newNode, int oldCol, int newCol);
	//Helper functions
	void addToFrontier(node toAdd);
	void nodeExpand(node current);
	void setup(vector<vector<int>> ship, int craneRow, int craneCol, int zone);
	bool stateExists(node toAdd);
	bool goalTest(vector<vector<int>> ship);

public:
	void search(vector<vector<int>> ship, int craneRow, int craneCol, int zone);
	void printSteps(vector<int> path);

	puzzles() { frontier = {}; closed = {}; };
};

void puzzles::printSteps(vector<int> path) {		//---------------------------------------TODO-----------------------------------------------
	cout << "Steps taken are: start";
	for (unsigned int i = 0; i < path.size(); i++) {
		cout << " -> ";
		string move = "";
		switch (path[i]) {
		case 0:
			move = "up";
			break;
		case 1:
			move = "down";
			break;
		case 2:
			move = "left";
			break;
		case 3:
			move = "right";
			break;
		}
		cout << move;
	}
}

bool puzzles::goalTest(vector<vector<int>> ship) {		//----------------------------------------------------------------TODO------------------------------------------------------------------------
	int expected = 1;
	int incorrect = 0;
	for (unsigned int i = 0; i < ship.size(); i++) {
		for (unsigned int j = 0; j < ship[0].size(); j++) {
			if (ship[i][j] != expected++ && (i != ship.size() - 1 || j != ship[0].size() - 1)) {	//ignore trailing 0
				return false;
			}
		}
	}
	return true;
}

void puzzles::setup(vector<vector<int>> ship, int craneRow, int craneCol, int zone) {
	expandedNodeCount = 0;
	maxQueueSize = 0;
	closed.clear();
	frontier.clear();
	node first;
	first.ship = ship;
	first.row = craneRow;
	first.col = craneCol;
	first.zone = zone;
	first.prev = {
		{-1, -1},
		{-1, -1},
		{-1, -1}
	};
	first.cost = 0;
	for (int i = 0; i < BUFFERHIGHT; i++) {
		vector<int> row = {};
		for (int j = 0; j < BUFFERWIDTH; j++) {
			row.push_back(0);
		}
		first.buffer.push_back(row);
	}
	heuristic(first, 0, 0);
	addToFrontier(first);
}

bool puzzles::stateExists(node toAdd) {
	for (unsigned int i = 0; i < closed.size(); i++) {
		if (toAdd.heur == closed[i].heur) {		//Identical states should have identical heur values
			bool miss = false;
			for (unsigned int j = 0; j < closed[i].ship.size(); j++) {
				if (true == miss)
					break;
				for (unsigned int k = 0; k < closed[i].ship[j].size(); k++) {
					if (toAdd.ship[j][k] != closed[i].ship[j][k]) {
						miss = true;
						break;
					}
				}
			}
			if (miss == false)
				return true;
		}
	}
	for (unsigned int i = 0; i < frontier.size(); i++) {
		if (toAdd.heur == frontier[i].heur) {		//Identical states should have identical heur values
			bool miss = false;
			for (unsigned int j = 0; j < frontier[i].ship.size(); j++) {
				if (true == miss)
					break;
				for (unsigned int k = 0; k < frontier[i].ship[j].size(); k++) {
					if (toAdd.ship[j][k] != frontier[i].ship[j][k]) {
						miss = true;
						break;
					}
				}
			}
			if (miss == false) {
				if (toAdd.cost < frontier[i].cost)
					frontier[i] = toAdd;
				return true;
			}
		}
	}
	return false;
}

void puzzles::search(vector<vector<int>> ship, int craneRow, int craneCol, int zone) {
	setup(ship, craneRow, craneCol, zone);

	while (true) {
		if (true == frontier.empty()) {
			cout << "\n\nFailed\n\n";
			cout << "To solve this problem the search algorithm expanded a total of " << expandedNodeCount << " nodes." << endl;
			cout << "The maximum number of nodes in the queue at any one time : " << maxQueueSize << endl;
			return;
		}
		if (0 == frontier[0].heur) {	//(true == goalTest(frontier[0].ship)) {
			cout << "\n\nFOUND\n\n" << endl;
			cout << "To solve this problem the search algorithm expanded a total of " << expandedNodeCount << " nodes." << endl;
			cout << "The maximum number of nodes in the queue at any one time : " << maxQueueSize << endl;
			cout << "The depth of the goal node was " << frontier[0].path.size() << endl;
			printPuzzle(frontier[0].ship);
			//printSteps(frontier[0].path);
			cout << "\n\n";
			return;
		}
		if (0 == expandedNodeCount++ % 100) {
			cout << ".";
		}
		nodeExpand(frontier[0]);
	}
}

void puzzles::addToFrontier(node toAdd) {
	if (true == stateExists(toAdd))
		return;
	if (frontier.size() + 1 > maxQueueSize)
		maxQueueSize = frontier.size() + 1;
	if (true == frontier.empty()) {
		frontier.push_back(toAdd);
		return;
	}
	for (unsigned int i = 0; i < frontier.size(); i++) {
		int toAddCost = toAdd.cost + toAdd.heur;
		int frontierCost = frontier[i].cost + frontier[i].heur;
		if (toAddCost < frontierCost) {
			frontier.emplace(frontier.begin() + i, toAdd);
			return;
		}
	}
	frontier.push_back(toAdd);
}

int findTopShip(vector<vector<int>> ship, int col) {
	for (int i = SHIPHIGHT - 1; i >= 0; i--) {
		if (1 <= ship[i][col])
			return i;
	}
	return -1;
}

//Finds the closest available space to put a container
tuple<int, int> findBufferPlace(vector<vector<int>> buffer) {
	for (int i = BUFFERWIDTH - 1; i >= 0; i--) {
		for (int j = BUFFERHIGHT - 2; j >= 0; j--) {	//Buffer zone does not allow walls	(choice for simplicity, not mandated)
			if (1 <= buffer[j][i]) {
				if (BUFFERHIGHT - 2 > j) {	//There is an empty space in the column
					return { i, j + 1 };
				}
				else	//The column is full
					break;
			}
		}
	}
	return { -1, -1 };
}

//Finds the closest available container in the buffer zone
int findBufferGet(vector<vector<int>> buffer) {
	for (int i = BUFFERHIGHT - 2; i >= 0; i--) {	//Buffer zone does not allow walls	(choice for simplicity, not mandated)
		if (1 <= buffer[i][BUFFERWIDTH - 1]) {
			return i;
		}
	}
	return -1;
}

//Find all the walls on the ship (Columns completly filled with containers)
//May be better to just return dockmost wall
vector<int> findWalls(vector<vector<int>> ship) {
	vector<int> walls;
	for (int i = 0; i < SHIPWIDTH; i++) {
		if (1 <= ship[SHIPHIGHT - 1][i]) {
			walls.push_back(i);
		}
	}
	return walls;
}

bool validPath(vector<vector<int>> ship, int startCol, int endCol) {
	vector<int> walls = findWalls(ship);
	for (unsigned int i = 0; i < walls.size(); i++) {
		if (startCol <= walls[i] && walls[i] < endCol || endCol <= walls[i] && walls[i] < startCol)	//ship portal accessablility is not checked elsewhere (thus startCol <= walls[i])
			return false;
	}

	return true;
}

void puzzles::nodeExpand(node current) {
	current.cost++;
	closed.push_back(frontier[0]);
	frontier.erase(frontier.begin());	//remove node being expanded from frontier
	int containerToMove = 0;	//-------------------------------------------------------------------TODO-----------------------------------------------------------------------
	vector<node> validNodes;
	for (int i = -1; i <= SHIPWIDTH; i++) {
		node newNode = current;
		if (-1 != heuristic(newNode, containerToMove, i)) {	//Move container from ship
			validNodes.push_back(newNode);
		}
		if (-1 != heuristic(newNode, -1, i)) {	//Move container from buffer
			validNodes.push_back(newNode);
		}
		if (-1 != heuristic(newNode, SHIPWIDTH, i)) {	//Load container from truck
			validNodes.push_back(newNode);
		}
	}

	for (unsigned int i = 0; i < validNodes.size(); i++) {
		if (stateExists(validNodes[i]))
			;	//------------------------------------------------------------------------------TODO--------------------------------------------------------------------
	}
}

/*
* if col == -1, col is buffer
* if col == SHIPWIDTH, col is truck
* returns -1 if move cannot be done
*/
int puzzles::heuristic(node &newNode, int oldCol, int newCol) {
	int oldTop = -1, newTop = -1, cost = 0, container = 0;
	//Find the top container (if portal is needed for grabbing container, cost to get back to ship is precalculated)
	if (SHIPWIDTH == oldCol) {	//Load from truck
		if (0 == newNode.numToLoad)
			return -1;	//No containers left to load
		if (-1 == newCol)
			return -1;	//Never load a container from truck to buffer
		oldTop = SHIPHIGHT - 1;
		if (0 == newNode.zone) {	//crane starts on ship
			if (false == validPath(newNode.ship, 0, newNode.col))
				return false;	//crane can't get to container
			cost = newNode.col + ((SHIPHIGHT - 1) - newNode.row);	//travel time to portal
			cost += PORTALTIME;	//portal travel time
		}
		else if (1 == newNode.zone) {	//crane starts in buffer
			cost = ((BUFFERWIDTH - 1) - newNode.col) + ((BUFFERHIGHT - 1) - newNode.row);	//travel time to portal
			cost += PORTALTIME;	//portal travel time
		}
		cost += PORTALTIME;	//time to get back to ship portal
		container = 2;	//Not going to be unloaded
		newNode.zone = 2;
	}
	else if (-1 != oldCol) {	//Moving from ship
		oldTop = findTopShip(newNode.ship, oldCol);
		if (-1 == oldTop)
			return -1;	//No containers to grab in column
		container = newNode.ship[oldTop][oldCol];
		newNode.ship[oldTop][oldCol] = 0;
		if (0 == newNode.zone) {	//crane starts on ship
			if (false == validPath(newNode.ship, oldCol, newCol))
				return false;	//crane can't get to container
			cost = (max(newNode.col, oldCol) - min(newNode.col, oldCol)) + ((SHIPHIGHT - 1) - newNode.row) + ((SHIPHIGHT -1) - oldTop);	//travel time to container (x dist + up dist + down dist)
		}
		else if (1 == newNode.zone) {	//crane starts in buffer
			if (false == validPath(newNode.ship, 0, oldCol))
				return false;	//crane can't get to container
			cost = ((BUFFERWIDTH - 1) - newNode.col) + ((BUFFERHIGHT - 1) - newNode.row) + oldCol + ((SHIPHIGHT - 1) - oldTop);	//travel time to portal (bx dist + bup dist + sx dist + sdown dist)
			cost += PORTALTIME;	//portal travel time
		}
		else if (2 == newNode.zone) {	//crane starts at trucks
			if (false == validPath(newNode.ship, 0, oldCol))
				return false;	//crane can't get to container
			cost = PORTALTIME;
		}
		newNode.zone = 0;
	}
	else {	//Moving from buffer
		int row = findBufferGet(newNode.buffer);
		if (-1 == row)
			return -1;	//No container in buffer
		oldTop = row;
		container = newNode.buffer[oldTop][oldCol];
		newNode.buffer[oldTop][oldCol] = 0;
		if (1 == newNode.zone) {	//crane starts in buffer
			cost = (max(newNode.col, oldCol) - min(newNode.col, oldCol)) + ((BUFFERHIGHT - 1) - newNode.row) + ((BUFFERHIGHT - 1) - oldTop);	//travel time to container (x dist + up dist + down dist)
		}
		else if (0 == newNode.zone) {	//crane starts on ship
			if (false == validPath(newNode.ship, 0, newNode.col))
				return false;	//crane can't get to container
			cost = newNode.col + ((SHIPHIGHT - 1) - newNode.row) + ((BUFFERWIDTH - 1) - oldCol) + ((BUFFERHIGHT - 1) - oldTop);	//travel time to portal (sx dist + sup dist + bx dist + bdown dist)
			cost += PORTALTIME;	//portal travel time
		}
		else if (2 == newNode.zone) {	//crane starts at trucks
			cost = PORTALTIME;
		}
		newNode.zone = 1;
	}

	newNode.prev[0][0] = oldCol;
	newNode.prev[0][1] = oldTop;
	newNode.prev[2][0] = newNode.zone;

	if (SHIPWIDTH == newCol) {	//moving to truck
		if (2 == newNode.zone)
			return -1;	//No moving from truck to truck;
		if (0 == newNode.zone) {	//container on ship
			if (false == validPath(newNode.ship, 0, oldCol))	//wall blocking path
				return -1;
			cost += ((SHIPHIGHT - 1) - oldTop) + oldCol + PORTALTIME;	// sup + sx + portal
		}
		else if (1 == newNode.zone) {	//container in buffer
			cost += ((BUFFERHIGHT - 1) - oldTop) + ((BUFFERWIDTH - 1) - oldCol) + PORTALTIME;	// bup + bx + portal
		}
		newNode.zone = 2;
	}
	else if (-1 != newCol) {	//moving to ship
		newTop = findTopShip(newNode.ship, newCol);
		if (-1 == newTop)
			return -1;	//Column is full
		if (0 == newNode.zone) {	//container on ship
			if (false == validPath(newNode.ship, oldCol, newCol))	//wall blocking path
				return -1;
			cost += ((SHIPHIGHT - 1) - oldTop) + (max(oldCol, newCol) - min(oldCol, newCol)) + ((SHIPHIGHT - 1) - newTop);	// sup + sx + sdown
		}
		else {
			if (false == validPath(newNode.ship, 0, oldCol))	//wall blocking path
				return -1;
			if (1 == newNode.zone) {	//container in buffer
				cost += ((BUFFERHIGHT - 1) - oldTop) + ((BUFFERWIDTH - 1) - oldCol) + PORTALTIME + newCol + ((SHIPHIGHT - 1) - newTop);	// bup + bx + portal + sx + sdown
			}
			else if (2 == newNode.zone) {	//container on truck
				cost += PORTALTIME + newCol + ((SHIPHIGHT - 1) - newTop);	// portal + sx + sdown
			}
		}
		newNode.ship[newTop][newCol] = container;
		newNode.zone = 0;
	}
	else {	//moving to buffer
		tuple<int, int> tmp = findBufferPlace(newNode.buffer);
		newCol = get<0>(tmp);
		newTop = get<1>(tmp);
		if (-1 == newCol)
			return -1;	//No space in buffer
		if (0 == newNode.zone) {	//container on ship
			if (false == validPath(newNode.ship, 0, oldCol))	//wall blocking path
				return -1;
			cost += ((SHIPHIGHT - 1) - oldTop) + oldCol + PORTALTIME + ((BUFFERWIDTH - 1) - newCol) + ((BUFFERHIGHT - 1) - newTop);	// sup + sx + portal + bx + bdown
		}
		newNode.buffer[newTop][newCol] = container;
		newNode.zone = 1;
	}

	newNode.prev[1][0] = newCol;
	newNode.prev[1][1] = newTop;
	newNode.prev[2][1] = newNode.prev[2][1] = newNode.zone;
	newNode.path.push_back(newNode.prev);

	newNode.cost += cost;


	int heur = 0; //# of unloadable containers + number of containers covering them
	return 0;
}

void printPuzzle(vector<vector<int>> ship) {
	for (unsigned int i = 0; i < ship.size(); i++) {
		for (unsigned int j = 0; j < ship[0].size(); j++) {
			int val = ship[i][j];
			if (0 == val)
				cout << "\t*";
			else
				cout << "\t" << val;

		}
		cout << "\n\n";
	}
}


//0 is empty space, 1 is container to remove, 2 is other container, -1 is out of bounds

void givenTests() {
	puzzles p = puzzles();
	vector<vector<int>> ship;
	ship = {		//Trivial
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nTrivial:\n";
	printPuzzle(ship);
	auto start = chrono::steady_clock::now();
	debug = false;
	p.search(ship, 0, 0, 0);
	auto finish = chrono::steady_clock::now();
	auto duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	ship = {		//Very Easy
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nVery Easy:\n";
	printPuzzle(ship);
	start = chrono::steady_clock::now();
	debug = false;
	p.search(ship, 0, 0, 0);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	ship = {		//Easy
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nEasy:\n";
	printPuzzle(ship);
	start = chrono::steady_clock::now();
	debug = false;
	p.search(ship, 0, 0, 0);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	ship = {		//Doable
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1},
		{1, 2, 0, 2, 1, 2, 0, 0, 0, 0, 0, 1},
		{1, 1, 0, 1, 2, 1, 0, 0, 0, 2, 2, 2}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nDoable:\n";
	printPuzzle(ship);
	start = chrono::steady_clock::now();
	debug = true;
	p.search(ship, 0, 0, 0);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	ship = {		//Oh Boy
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nOh Boy:\n";
	printPuzzle(ship);
	start = chrono::steady_clock::now();
	debug = false;
	p.search(ship, 0, 0, 0);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	ship = {		//Impossible
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nImpossible:\n";
	printPuzzle(ship);
	start = chrono::steady_clock::now();
	debug = false;
	p.search(ship, 0, 0, 0);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;
}


int main() {
	puzzles p = puzzles();
	vector<vector<int>> ship;
	
	givenTests();

	//Stops the console from automatically closing when finished
	int x;
	cin >> x;
	cout << x;
	return 0;
}