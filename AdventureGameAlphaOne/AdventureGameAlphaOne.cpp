// AdventureGameAlphaOne.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;

// states for game status
enum game_states {
	STATE_EXIT=0,
	STATE_MENU,
	STATE_INIT,
	STATE_PLAYING,
	STATE_GAMEOVER
};

// states for main menu
enum menu_states {
	MENU_HISCORES=1,
	MENU_PLAY,
	MENU_EXIT
};

// states for commands entered
enum game_commands {
	COMMAND_UNKNOWN = 0,
	COMMAND_CONTINUE,
	COMMAND_FIGHT,
	COMMAND_SEARCH,
	COMMAND_GO,
	COMMAND_LOOK,
	COMMAND_DODGE,
	COMMAND_RUN,
	COMMAND_ESCAPE
};

// states for different types of monsters, organized by difficult(-5 to 5) -10 for not_a_monster and 10 for final boss
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
	MONSTER_OGRE,
	MONSTER_TROLL_KING=10,
	MONSTER_RANDOM
};

// tags for different rooms, each corresponding to a room within the room array
enum game_locations {
	FOREST = 0,
	FOREST_2,
	FOREST_3,
	ENTRY,
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

enum game_traps {
	TRAP_NONE = 0,
	TRAP_NORMAL
};

enum game_items {
	ITEM_FIST = -10,
	ITEM_NONE = 0,
	ITEM_RED_KEY,
	ITEM_BLUE_KEY,
	ITEM_YELLOW_KEY,
	ITEM_GREEN_KEY,
	ITEM_ARMOR_1,
	ITEM_SWORD_1
};


struct game_item {
	string name;
	string describer;
	string description;
	int type = ITEM_NONE;
};

game_item createItem(int);


// function prototypes
vector<string> delimit(const string &, char);
bool isMatch(string, string);
int determineAction(vector<string>);
vector<string> getInput();
bool isOpeningRoom(int);
bool isNeutralRoom(int);
void initRooms();
int d20(int);

// action function prototypes
void doContinue();
void doFight();
void doDodge();
void doLook();
void doRun();




// player struct - self explanatory
struct game_player {
public:
	string name;
	int bonus;
	long gold;
	int HP;
	int rloc;
	game_item weapon;

	game_player() {
		bonus = 5;
		gold = 0;
		rloc = FOREST;
		weapon = createItem(ITEM_FIST);
		HP = 3;
	}

};

// monster struct, for fleshing out monsters
struct game_monster {
	string name, description, describer;
	int HP;
	game_item weapon;
	int difficulty;

	game_monster() {
		difficulty = MONSTER_NONE;
		HP = 0;
		weapon = createItem(ITEM_FIST);
	}
};

game_monster createMonster(int = MONSTER_NONE);

struct game_trap {
	string name, description, describer;
	bool isArmed = false;
	int difficulty = TRAP_NONE;
};

// room struct, for fleshing out rooms and their attributes
struct game_room {
	bool hasRelic, beenVisited;
	string name, description, describer;
	game_monster monster;
	vector<game_items> items;
	game_trap trap;

	game_room() {
		name = description = "";
		hasRelic = beenVisited = false;
	}

	game_room(string name, string description, bool hasRelic) {
		this->name = name;
		this->description = description;
		this->hasRelic = hasRelic;
	}
};

// custom type functions


template<class T>
string getWeapon(T & entity) {
	return entity->weapon.name;
};

template<class T>
bool isLiving(T & entity) {
	return (entity->HP > 0);
};

// constant for rooms array
const int GAME_ROOM_SIZE = EXIT+1;

// globals
game_player *player;
game_room *rooms;
int gameState;

int main()
{
	// initialize randomizer
	srand(time(0));

	// initialize program variables
	int choice, action=0;
	string input;
	vector<string> command;

	// main game loop
	gameState = STATE_MENU;
	while(gameState!=STATE_EXIT) {
		switch(gameState) {
			// main menu
			case STATE_MENU:
				cout << "Please enter a choice:" << endl;
				cout << "1: Show high-scores" << endl;
				cout << "2: Play game" << endl;
				cout << "3: Exit" << endl;
				command = getInput();
				choice = atoi(command[0].c_str());
				if(choice) {
					switch(choice) {
						case MENU_HISCORES:
							cout << "High scores not implemented yet... sorry" << endl;
						break;
						case MENU_PLAY:
							cout << "Game initializing..." << endl;
							gameState = STATE_INIT;
						break;
						case MENU_EXIT:
							cout << "Thank you for playing dungeon delve!" << endl;
							gameState = STATE_EXIT;
						break;
						default:
							cout << "Invalid choice." << endl;
						break;
					}
				}
			break;
			// initialize game - this is an intermediary state
			case STATE_INIT:
				player = new game_player();
				initRooms();

				// show first room
				doLook();
				gameState = STATE_PLAYING;
			break;
			// game is running
			case STATE_PLAYING:
				if (!isLiving(player)) {
					// death is handled here
					cout << "You have died!" << endl;
					gameState = STATE_GAMEOVER;
					continue;
				}
				command = getInput();
				action = determineAction(command);
				// identify target
				if (action && player->HP) {
					switch (action) {
						case COMMAND_CONTINUE:
							doContinue();
						break;
						case COMMAND_GO:
						break;
						case COMMAND_LOOK:
							doLook();
						break;
						case COMMAND_FIGHT:
							doFight();
						break;
						case COMMAND_DODGE:
							doDodge();
						break;
						case COMMAND_RUN:
							doRun();
						break;
						case COMMAND_ESCAPE:
						break;
						case COMMAND_UNKNOWN:
						break;
						default:
						break;
					}
				}
			break;
			// player died
			case STATE_GAMEOVER:
				// cleanup
				delete[] rooms;
				delete player;
				gameState = STATE_MENU;
			break;
		}
	}
}

// doFight() - do combat between player and monster
void doFight() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	if (isOpeningRoom(p->rloc)) {
		cout << "Fight? Fight the urge to fight and just RUN!" << endl;
	} else if (m->difficulty == MONSTER_NONE) {
		cout << "There is nothing to fight!" << endl;
	} else if(!isLiving(m)) {
		cout << "But the " + m->name + " is already dead!" << endl;
	} else {
		int pHit = d20(player->bonus);
		int mHit = d20(m->difficulty);
		cout << "You rolled: " << pHit << endl;
		cout << m->name << " rolled: " << mHit << endl;
		if(pHit >= mHit) {
			cout << "You out-fox the " + m->name + " and land a solid blow with your " << getWeapon(p) << "!";
			m->HP--;
			if(m->HP <= 0) {
				cout << "The " + m->name + " crumbles to the ground lifelessly." << endl;
			}
		} else if(mHit > pHit) {
			cout << "The " + m->name + " slips into flanking position and lands a mighty blow with its " << getWeapon(m) << " that sends you flying!" << endl;
			player->HP--;
		}

	}
};

// doDodge() 
void doDodge() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if (isOpeningRoom(p->rloc)) {
		cout << "There's no time to look for traps.... just RUN!" << endl;
	} else if (t->isArmed) {
		if(m->difficulty != MONSTER_NONE && isLiving(m)) {
			cout << "You attempt to dodge through the traps with the " + m->name + "in hot pursuit!" << endl;
		} else {
			cout << "You attempt to dodge through the traps!" << endl;
		}
		int pSuccess = d20(player->bonus);
		int mSuccess = d20(0);
		cout << "You rolled: " << pSuccess << endl;
		cout << "Trap rolled: " << mSuccess << endl;
		if (pSuccess >= mSuccess) {
			cout << "You safely manuever through the trap!" << endl;
			t->isArmed = false;
		} else if (mSuccess > pSuccess) {
			cout << "You failed to dodge the trap!" << endl;
			player->HP--;
		}
	} else {
		cout << "But there are no traps!" << endl;
	}
};

void doContinue() {
	game_room *r = &rooms[player->rloc];
	game_player *p = player;
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if(isOpeningRoom(p->rloc)) {
		cout << "There's no time for walking... RUN!" << endl;
	} else if(isLiving(m)) {
		cout << "You can't leave until you've vanquished the " + r->monster.name + "!";
	} else if(t->isArmed) {
		cout << "The room is trapped! Are you trying to die?";
	} else {
		r->beenVisited = true;
		if(p->rloc == RED_EXIT || p->rloc == GREEN_EXIT || p->rloc == BLUE_EXIT || p->rloc == YELLOW_EXIT) p->rloc = HALLWAY;
		else p->rloc++;
		r = &rooms[player->rloc];
		m = &r->monster;
		cout << "You continue on into the next area..." << endl;
		if (isLiving(m)) {
			cout << "You spot a " << m->name << "!" << endl;
		} else if (t->isArmed) {
			cout << "You spot some traps!" << endl;
		}
	}
};

void doLook() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	cout << r->name << endl;
	cout << r->description << endl;
	if (isOpeningRoom(p->rloc)) {
		cout << "It's all a blur.... keep RUNNING!" << endl;
	} else {
		if (isLiving(m)) {
			cout << "A " << m->describer << " stands before you." << endl;
		} else if(!isLiving(m) && m->difficulty != MONSTER_NONE) {
			cout << "The corpse of a " << m->name << " lies motionless." << endl;
		}
		if (t->isArmed) {
			cout << "You spot some traps!" << endl;
		}
	}
}

void doRun() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if(isOpeningRoom(p->rloc)) {
		p->rloc++;
		doLook();
	} else {
		cout << "RUN not implemented here yet" << endl;
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
	} else if (isMatch("look", cmd)) {
		cid = COMMAND_LOOK;
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

bool isNeutralRoom(int r) {
	if (r == FOREST || r == FOREST_2 || r == FOREST_3 || r == ENTRY || r == HALLWAY || \
		r == RED_EXIT || r == GREEN_EXIT || r == BLUE_EXIT || r == YELLOW_EXIT || \
		r == BOSS_ENTRY || r == EXIT) return true;
	return false;
}

bool isOpeningRoom(int r) {
	if (r == FOREST || r == FOREST_2 || r == FOREST_3) return true;
	return false;
}


void initRooms() {
	rooms = new game_room[GAME_ROOM_SIZE];
	for(int i=0;i < GAME_ROOM_SIZE; i++) {
		game_room *r = &rooms[i];
		game_monster *m = &r->monster;
		game_trap *t = &rooms[i].trap;
		if(!isNeutralRoom(i)) {
			t->isArmed = rand() % 2;
			r->hasRelic = rand() % 2;
			if(rand() % 2) {
				*m = createMonster(MONSTER_RANDOM);
			}
		}
	}
	// neutral rooms
	rooms[FOREST].name = "[Running through the forest]";
	rooms[FOREST].description = "In the distance, you hear a faint siren echoing through the trees, and footsteps getting louder.\
	\nThe trolls are after you. You barely managed to slip out alive after they knocked your door down, but they were in fast pursuit\
	As you escaped your village, they had already slain the chief and were rounding people up. Anyone who tried to run was killed, but\
	you got a head-start. Your only hope now is that your feet rumble faster than their bellies.";
	rooms[FOREST_2].description = "Placeholder text for forest running";
	rooms[FOREST_3].description = "Placeholder text for forest running";
	rooms[ENTRY].name = "[Placeholder entry name]";
	rooms[ENTRY].description = "Placeholder text here for entry";
	rooms[HALLWAY].name = "Corridor, Chamber";
	rooms[HALLWAY].name = "Placeholder text for colored-door corridor";
	rooms[RED_EXIT].name = "Red Door, Narrow Passage";
	rooms[RED_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[GREEN_EXIT].name = "Green Door, Narrow Passage";
	rooms[GREEN_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[BLUE_EXIT].name = "Blue Door, Narrow Passage";
	rooms[BLUE_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[YELLOW_EXIT].name = "Yellow Door, Narrow Passage";
	rooms[YELLOW_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";

	// populated rooms - TODO
};


// createMonster - spawns a critter and assigns it stats
game_monster createMonster(int id) {
	game_monster m;
	int newMob;
	if(id == MONSTER_RANDOM) newMob = (rand() % 10) - 5;
	else newMob = id;
	m.difficulty = newMob;
	if(m.difficulty < 0) m.HP = 1;
	else if(m.difficulty < 4) m.HP = 2;
	else if(m.difficulty < 10) m.HP = 3;
	switch (newMob) {
		case MONSTER_GREEN_GOBLIN:
			m.name = "goblin";
			m.describer = "small green goblin";
			break;
		case MONSTER_RED_GOBLIN:
			m.name = "goblin";
			m.describer = "red goblin";
			break;
		case MONSTER_PURPLE_GOBLIN:
			m.name = "goblin";
			m.describer = "large purple goblin";
			break;
		case MONSTER_SMALL_SKELETON:
			m.name = "skeleton";
			m.describer = "small skeleton";
			break;
		case MONSTER_KOBOLD:
			m.name = "kobold";
			m.describer = "vicious kobold";
			break;
		case MONSTER_SKELETON:
			m.name = "skeleton";
			m.describer = "bleached skeleton";
			break;
		case MONSTER_GNOLL:
			m.name = "gnoll";
			m.describer = "large gnoll";
			break;
		case MONSTER_ORC:
			m.name = "orc";
			m.describer = "nasty orc";
			break;
		case MONSTER_LARGE_SKELETON:
			m.name = "skeleton";
			m.describer = "giant skeleton";
			break;
		case MONSTER_BUGBEAR:
			m.name = "bugbear";
			m.describer = "greasy-looking bugbear";
			break;
		case MONSTER_OGRE:
			m.name = "ogre";
			m.describer = "massive ogre";
			break;
		default:
			m.name = "error";
			m.describer = "error";
			cout << "Unknown critter type: " << newMob << endl;
		break;
	}
	return m;
};

game_item createItem(int id) {
	game_item newItem;
	switch(id) {
		case ITEM_FIST:
			newItem.name = "fists";
			newItem.describer = "fists";
			newItem.description = "Your old chums, law and order. They haven't let you down yet.";
			newItem.type = ITEM_FIST;
	}
	return newItem;
};



// d20 function - returns a roll from 1 to 20, +/- bias
int d20(int bias) {
	int roll = rand() % 20 + 1;
	roll += bias;
	(roll > 20 ? roll = 20 : 0);
	return roll;
}