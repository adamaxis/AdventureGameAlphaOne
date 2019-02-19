// AdventureGameAlphaOne.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;


bool running;


struct game_object {
	string name, description, noun;
	vector<string> adjectives;
	int TYPE;
};

struct game_player {
public:
	string name;
	long gold;
	vector<game_object *> items;
	int rloc;
};

struct game_room {
	vector<game_object *> items;
	vector<game_room *> exits;
	game_player *player;
};

enum game_commands {
	COMMAND_UNKNOWN = 0,
	COMMAND_NORTH,
	COMMAND_SOUTH,
	COMMAND_EAST,
	COMMAND_WEST,
	COMMAND_IN,
	COMMAND_OUT,
	COMMAND_UP,
	COMMAND_DOWN,
	COMMAND_GO,
	COMMAND_LOOK,
	COMMAND_TAKE,
	COMMAND_GET,
	COMMAND_ATTACK,
	COMMAND_RUN,
	COMMAND_DODGE
};

vector<string> delimit(const string &, char);
bool isMatch(string, string);
void parse(vector<string>, void *);

template<class T>
void find(string s, T & object) {
	if(object.cmd != 1) {
		cout << "Not equal to 1";
	}
};

template<class T>
void move(vector<T> & o1, vector<T> & o2, int index) {
	if(o1 == nullptr) {
		cout << "ERROR: o1 NULL" << endl;
		return;
	}
	if (o2 == nullptr) {
		cout << "ERROR: o2 NULL" << endl;
		return;
	}
	if(o1.at(index) == nullptr) {
		cout << "ERROR: nonexistent index" << endl;
		return;
	}

	o2.push_back(o1.at(index));
	o1.erase(index);
};


const int GAME_ROOM_SIZE = 1000;
const int GAME_OBJECT_SIZE = 1000;

game_player *player;
game_room *rooms;
game_object *objects;


int main()
{
	player = new game_player();
	rooms = new game_room[GAME_ROOM_SIZE];
	objects = new game_object[GAME_OBJECT_SIZE];

	running = true;
	string input;
	while(running) {
		cin >> input;
		vector<string> parsed = delimit(input, ' ');
		parse(parsed, 0);
		//for (auto i : parsed) cout << i << endl;
			//if(input) {
				
			//}
				


	}

}

vector<string> delimit(const string &toSplit, char delim) {
	vector<string> result;
	vector<game_player> x;
	stringstream stringStream (toSplit);
	string item;

	while (getline(stringStream, item, delim)) {
		result.push_back(item);
	}

	return result;
}

void parse(vector<string> input, void * src) {
	string cmd, target;
	int cid;
	// identify command
	cmd = input.at(0);
	if(isMatch("north", cmd)) {
		cout << "North";
		cid = COMMAND_NORTH;
	} else if (isMatch("sorth", cmd)) {
		cout << "South";
		cid = COMMAND_SOUTH;
	} else if (isMatch("east", cmd)) {
		cout << "East";
		cid = COMMAND_EAST;
	} else if (isMatch("west", cmd)) {
		cout << "West";
		cid = COMMAND_WEST;
	} else if (isMatch("up", cmd)) {
		cout << "Up";
		cid = COMMAND_UP;
	} else if (isMatch("down", cmd)) {
		cout << "Down";
		cid = COMMAND_DOWN;
	} else if (isMatch("go", cmd)) {
		cout << "Go";
		cid = COMMAND_GO;
	} else if (isMatch("out", cmd)) {
		cout << "Out";
		cid = COMMAND_OUT;
	} else if (isMatch("out", cmd)) {
		cout << "Look";
		cid = COMMAND_LOOK;

	} else {
		cout << "Unknown command";
		cid = COMMAND_UNKNOWN;
	}

	// identify target
	if(cid) {
		switch(cid) {
			case COMMAND_NORTH:
				for (auto i : rooms[player->rloc].exits)
					if(i) {

					}
			break;
			case COMMAND_SOUTH:
			break;
			case COMMAND_EAST:
			break;
			case COMMAND_WEST:
			break;
			case COMMAND_UP:
			break;
			case COMMAND_DOWN:
			break;
			case COMMAND_OUT:
			break;
			case COMMAND_LOOK:
			break;
			case COMMAND_GO:
			break;
			default:
			break;
	}

	}
}

bool isMatch(string src, string text) {
	int num = src.find(text, 0);
	if(num != -1 && num == 0) return true;
	return false;
};