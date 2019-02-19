// AdventureGameAlphaOne.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;

enum game_commands {
	COMMAND_UNKNOWN = 0,
	COMMAND_CONTINUE,
	COMMAND_FIGHT,
	COMMAND_SEARCH,
	COMMAND_GO,
	COMMAND_DODGE,
	COMMAND_RUN,
	COMMAND_ESCAPE
};

enum game_monsters {
	MONSTER_NONE = -10,
	MONSTER_GREEN_GOBLIN = -5,
	MONSTER_SMALL_SKELETON,
	MONSTER_RED_GOBLIN,
	MONSTER_KOBOLD,
	MONSTER_SKELETON,
	MONSTER_GNOLL,
	MONSTER_ORC,
	MONSTER_PURPLE_GOBLIN,
	MONSTER_LARGE_SKELETON,
	MONSTER_BUGBEAR,
	MONSTER_OGRE
};

enum game_locations {
	ENTRY = 0,
	HALLWAY,
	RED_1,
	RED_2,
	RED_3,
	RED_4,
	RED_5,
	RED_EXIT,
	GREEN_1,
	GREEN_2,
	GREEN_3,
	GREEN_4,
	GREEN_5,
	GREEN_EXIT,
	BLUE_1,
	BLUE_2,
	BLUE_3,
	BLUE_4,
	BLUE_5,
	BLUE_EXIT,
	YELLOW_1,
	YELLOW_2,
	YELLOW_3,
	YELLOW_4,
	YELLOW_5,
	YELLOW_EXIT,
	BOSS_ENTRY,
	BOSS,
	EXIT
};


struct game_player {
public:
	string name;
	int bonus;
	long gold;
	bool isLiving=true;
	int rloc;
};

struct game_monster {
	string name, description, describer;
	bool isLiving=false;
	int difficulty=MONSTER_NONE;
};

struct game_room {
	bool hasTraps, hasRelic, beenVisited;
	string name, description, describer;
	game_monster monster;

	game_room() {
		name = description = "";
		hasTraps = hasRelic = beenVisited = false;
	}

	game_room(string name, string description, bool hasTraps, bool hasRelic) {
		this->name = name;
		this->description = description;
		this->hasTraps = hasTraps;
		this->hasRelic = hasRelic;
	}
};

vector<string> delimit(const string &, char);
bool isMatch(string, string);
int determineAction(vector<string>);
vector<string> getInput();
void doContinue();
void doFight();
void doDodge();
void doLook();
void initRooms();
int d20(int);

/*template<class T>
void find(string s, T & object) {
	if(object.cmd != 1) {
		cout << "Not equal to 1";
	}
};*/


const int GAME_ROOM_SIZE = EXIT+1;

int gameState;

game_player *player;
game_room *rooms;


int main()
{
	srand(time(0));
	player = new game_player();
	initRooms();

	gameState = 1;
	string input;
	while(gameState!=0) {
		if (!player->isLiving) {
			cout << "You have died!" << endl;
			gameState = 0;
			continue;
		}
		vector<string> input = getInput();
		int action = determineAction(input);
		// identify target
		if (action && player->isLiving) {
			switch (action) {
				case COMMAND_CONTINUE:
					doContinue();
					break;
				case COMMAND_GO:
					break;
				case COMMAND_FIGHT:
					doFight();
					break;
				case COMMAND_DODGE:
					doDodge();
					break;
				case COMMAND_RUN:
					break;
				case COMMAND_ESCAPE:
					break;
				case COMMAND_UNKNOWN:
					break;
				default:
					break;
			}
		}
	}
	delete [] rooms;
	delete player;
}

void doFight() {
	game_room *r = &rooms[player->rloc];
	game_monster *m = &r->monster;
	if (m->difficulty == MONSTER_NONE) {
		cout << "There is nothing to fight!" << endl;
	} else if(!m->isLiving) {
		cout << "But the " + m->name + " is already dead!" << endl;
	} else {
		int pHit = d20(player->bonus);
		int mHit = d20(m->difficulty);
		if(pHit >= mHit) {
			cout << "The " + m->name + " crumbles to the ground lifelessly." << endl;
			m->isLiving = false;
		} else if(mHit > pHit) {
			cout << "The " + m->name + " lands a mighty blow that sends you flying!" << endl;
			player->isLiving = false;
		}

	}
};

void doDodge() {
	game_room *r = &rooms[player->rloc];
	game_monster *m = &r->monster;
	if (r->hasTraps) {
		if(m->difficulty != MONSTER_NONE && m->isLiving) {
			cout << "You attempt to dodge through the traps with the " + m->name + "in hot pursuit!" << endl;
		} else {
			cout << "You attempt to dodge through the traps!" << endl;
		}
		int pSuccess = d20(player->bonus);
		int mSuccess = d20(m->difficulty);
		if (pSuccess >= mSuccess) {
			cout << "The " + m->name + " crumbles to the ground lifelessly." << endl;
			m->isLiving = false;
		} else if (mSuccess > pSuccess) {
			cout << "The " + m->name + " lands a mighty blow that sends you flying!" << endl;
			player->isLiving = false;
		}
	} else {
		cout << "But there are no traps!" << endl;
	}
};

void doContinue() {
	game_room *r = &rooms[player->rloc];
	game_player *p = player;
	game_monster *m = &r->monster;
	if(m->isLiving) {
		cout << "You can't leave until you've vanquished the " + r->monster.name + "!";
	} else if(r->hasTraps) {
		cout << "The room is trapped! Are you trying to die?";
	} else {
		r->beenVisited = true;
		if(p->rloc == RED_EXIT || p->rloc == GREEN_EXIT || p->rloc == BLUE_EXIT || p->rloc == YELLOW_EXIT) p->rloc = HALLWAY;
		else p->rloc++;
		r = &rooms[player->rloc];
		m = &r->monster;
		cout << "You continue on into the next area..." << endl;
		if (m->isLiving) {
			cout << "You spot an enemy!" << endl;
		} else if (r->hasTraps) {
			cout << "You spot some traps!" << endl;
		}
	}
};

void doLook() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	cout << r->name << endl;
	cout << r->description << endl;
	if (m->isLiving && m->difficulty != MONSTER_NONE) {
		cout << "You spot a " << m->describer << "!" << endl;
	}
	if (r->hasTraps) {
		cout << "You spot some traps!" << endl;
	}
}

vector<string> getInput() {
	string input;
	cin >> input;
	cin.clear();
	cin.ignore(10000, '\n');
	return delimit(input, ' ');
};

vector<string> delimit(const string &toSplit, char delim) {
	vector<string> result;
	stringstream stringStream (toSplit);
	string item;

	while (getline(stringStream, item, delim)) {
		result.push_back(item);
	}

	return result;
}

int determineAction(vector<string> input) {
	string cmd, target;
	int cid;
	// identify command
	cmd = input.at(0);
	if(isMatch("fight", cmd)) {
		cid = COMMAND_FIGHT;
	} else if (isMatch("run", cmd)) {
		cid = COMMAND_RUN;
	} else if (isMatch("escape", cmd)) {
		cid = COMMAND_ESCAPE;
	} else if (isMatch("dodge", cmd)) {
		cid = COMMAND_DODGE;
	} else if (isMatch("continue", cmd)) {
		cid = COMMAND_CONTINUE;
	} else {
		cout << "Unknown command";
		cid = COMMAND_UNKNOWN;
	}
	return cid;
};

bool isMatch(string src, string text) {
	int num = src.find(text, 0);
	if(num != -1 && num == 0) return true;
	return false;
};

void initRooms() {
	rooms = new game_room[GAME_ROOM_SIZE];
	for(int i=0;i < GAME_ROOM_SIZE; i++) {
		game_room *r = &rooms[i];
		if(i != ENTRY && i != HALLWAY && i != RED_EXIT && i != GREEN_EXIT && i != BLUE_EXIT && i != YELLOW_EXIT && i != BOSS_ENTRY && i != EXIT) {
			r->hasTraps = rand() % 2;
			r->hasRelic = rand() % 2;
			if(rand() % 2) {
				int newMob = (rand() % 10) - 5;
				game_monster *m = &r->monster;
				m->difficulty = newMob;
				m->isLiving = true;
				switch(newMob) {
					case MONSTER_GREEN_GOBLIN:
						m->name = "goblin";
						m->describer = "small green goblin";
					break;
					case MONSTER_RED_GOBLIN:
						m->name = "goblin";
						m->describer = "red goblin";
					break;
					case MONSTER_PURPLE_GOBLIN:
						m->name = "goblin";
						m->describer = "large purple goblin";
					break;
					case MONSTER_SMALL_SKELETON:
						m->name = "skeleton";
						m->describer = "small skeleton";
					break;
					case MONSTER_KOBOLD:
						m->name = "kobold";
						m->describer = "vicious kobold";
					break;
					case MONSTER_SKELETON:
						m->name = "skeleton";
						m->describer = "bleached skeleton";
					break;
					case MONSTER_GNOLL:
						m->name = "gnoll";
						m->describer = "large gnoll";
					break;
					case MONSTER_ORC:
						m->name = "orc";
						m->describer = "nasty orc";
					break;
					case MONSTER_LARGE_SKELETON:
						m->name = "skeleton";
						m->describer = "giant skeleton";
					break;
					case MONSTER_BUGBEAR:
						m->name = "bugbear";
						m->describer = "greasy-looking bugbear";
					break;
					case MONSTER_OGRE:
						m->name = "ogre";
						m->describer = "massive ogre";
					break;
					default:
						cout << "Unknown critter type: " << newMob << endl;
				}
			}
		}
	}
	// neutral rooms
	rooms[HALLWAY].name = "Corridor, Chamber";
	rooms[RED_EXIT].name = "Red Door, Narrow Passage";
	rooms[RED_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[GREEN_EXIT].name = "Green Corridor, Narrow Passage";
	rooms[GREEN_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[BLUE_EXIT].name = "Blue Corridor, Narrow Passage";
	rooms[BLUE_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[YELLOW_EXIT].name = "Yellow Corridor, Narrow Passage";
	rooms[YELLOW_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";

	// populated rooms
};

int d20(int bias) {
	int roll = rand() % 20 + 1;
	roll += bias;
	(roll > 20 ? roll = 20 : 0);
	return roll;
}