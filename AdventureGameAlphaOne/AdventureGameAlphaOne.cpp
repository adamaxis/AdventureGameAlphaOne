// AdventureGameAlphaOne.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <bitset>
using namespace std;

#define setB(x,y) x |= (1 << y)

#define clearB(x,y) x &= ~(1<< y)

bool checkB(int x, int y) {
	return (0 == (x & (1 << y))) ? false : true;
};
//#define checkB(x,y) ((0u == (x & (1<<y)))?false:true)

#define toggleB(x,y) (x ^= (1<<y))

enum item_states {
	BIT_IMMOBILE = 1,
	BIT_EXIT = 2,
	BIT_POTION = 3,
	BIT_WEAPON = 4,
	BIT_ARMOR = 5,
};

enum room_states {
	BIT_DARK = 1,
	BIT_RUNNING = 2,
	BIT_NOEXIT = 3
};

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
	COMMAND_USE,
	COMMAND_DODGE,
	COMMAND_TAKE,
	COMMAND_RUN,
	COMMAND_ESCAPE,
	COMMAND_ENTER
};

// states for different types of objects
enum game_object_types {
	TYPE_NONE = 0,
	TYPE_ITEM,
	TYPE_MONSTER,
	TYPE_PLAYER,
	TYPE_ROOM
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
	PIT_1,
	PIT_2,
	ENTRY,
	CORRIDOR,
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
	ITEM_CLAW,
	ITEM_NONE = 0,
	ITEM_RED_KEY,
	ITEM_BLUE_KEY,
	ITEM_YELLOW_KEY,
	ITEM_GREEN_KEY,
	ITEM_RED_DOOR,
	ITEM_BLUE_DOOR,
	ITEM_YELLOW_DOOR,
	ITEM_GREEN_DOOR,
	ITEM_GOLD_DOOR,
	ITEM_HOLE,
	ITEM_WEAPON=100,
	ITEM_DAGGER,
	ITEM_SCIMITAR,
	ITEM_RAPIER,
	ITEM_LONGSWORD,
	ITEM_BROADSWORD,
	ITEM_2HSWORD,
	ITEM_WEAPON_END,
	ITEM_ARMOR=200
};


struct game_item {
	string name;
	string adjectives;
	string description;
	int type = ITEM_NONE;
	long flags = 0;
	int prop1 = 0;
	int prop2 = 0;
	int id;

	game_item() {
		id = rand();
	}

	void clear() {
		name = adjectives = description = "";
		type = ITEM_NONE;
		flags = 0;
		prop1 = 0;
		prop2 = 0;
	}

};

game_item createItem(int);


// function prototypes
vector<string> delimit(const string &, char);
string trimFirst(string &, char);
bool isMatch(string, string);
int determineAction(vector<string>);
void resolveTarget(vector<string>);
vector<string> getInput();
bool isOpeningRoom(int);
bool isNeutralRoom(int);
void initRooms();
int d20(int = 0);

// action function prototypes
void doRun();
void doContinue();
void doGo();
void doFight();
void doDodge();
void doLook();
void doLookBrief();
void doUse();
void doTake();




// player struct - self explanatory
struct game_player {
public:
	string name;
	int bonus;
	long gold;
	int HP;
	int rloc;
	vector<game_item> items;
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
	string name, description, adjectives;
	int HP;
	game_item weapon;
	vector<game_item> items;
	int difficulty;

	game_monster() {
		difficulty = MONSTER_NONE;
		description = "none";
		HP = 0;
		weapon = createItem(ITEM_FIST);
	}
};

void monsterDeath(game_monster *);
game_monster createMonster(int = MONSTER_NONE);

struct game_trap {
	string name, description, adjectives;
	bool isArmed = false;
	int difficulty = TRAP_NONE;
};

// room struct, for fleshing out rooms and their attributes
struct game_room {
	bool hasRelic, beenVisited;
	string name, description, transition;
	game_monster monster;
	vector<game_item> items;
	long flags;
	game_trap trap;

	game_room() {
		name = description = transition = "";
		hasRelic = beenVisited = false;
	}

	game_room(string name, string description, bool hasRelic) {
		this->name = name;
		this->description = description;
		this->hasRelic = hasRelic;
	}
};

// custom type functions


struct game_command {
	int cmd = COMMAND_UNKNOWN;
	int type=0, ptype=0;
	void *target, *parent;
};

template<class T>
string getWeapon(T *entity) {
	return entity->weapon.name;
};

template<class T>
int getDamage(T *entity) {
	return entity->weapon.prop1;
};



template<class T>
string getDescriber(T *entity) {
	return (entity->adjectives + " " + entity->name);
};

template<class T>
string getName(T & entity) {
	return (entity->name);
};


template<class T>
string getDescription(T & entity) {
	return (entity->description);
};

template<class T>
bool areEqual(T *entity, T *entity2) {
	return(entity->id == entity2->id);
};

template<class T>
bool isLiving(T & entity) {
	return (entity->HP > 0);
};

bool isActive(game_trap *t) {
	return t->isArmed;
};

bool isExit(game_item *i) {
	if(checkB(i->flags, BIT_EXIT)) return true;
	return false;
};

// constant for rooms array
const int GAME_ROOM_SIZE = EXIT+1;

// globals
game_player *player;
game_room *rooms;
game_command target;

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
				switch(choice) {
					case MENU_HISCORES:
						/*
						ifstream inFile("Hiscore.txt");
						if (!inFile) {
							cout << "There are no High Scores yet. Please play the game to add a High Score";
						} else {
							for (int i= 0; i < 5; i++) {
							inFile >> hiScore[i];
							cout >> hiScore[i];
							inFile.close();
						}
						*/
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
			break;
			// initialize game - this is an intermediary state
			case STATE_INIT:
				player = new game_player();
				initRooms();

				// show first room
				cout << rooms[player->rloc].name << endl;
				cout << rooms[player->rloc].description << endl;
				// game = running
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
				// clear target
				target.type = TYPE_NONE;
				target.target = nullptr;
				command = getInput();
				action = determineAction(command);
				resolveTarget(command);
				// identify target
				if (action && isLiving(player)) {
					switch (action) {
						case COMMAND_CONTINUE:
							doContinue();
						break;
						case COMMAND_GO:
							doGo();
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
						case COMMAND_USE:
							doUse();
						break;
						case COMMAND_TAKE:
							doTake();
						break;
						case COMMAND_ESCAPE:
						break;

						case COMMAND_ENTER:
							
						break;
						case COMMAND_UNKNOWN:
						default:
							cout << "Unknown command" << endl;
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


template <class T>
bool adjCheck(vector<string> in, T *obj) {
	if(in.size() == 0) return true;
	for(string c : in) {
		if(c.compare("at") == 0) continue;
		if (c.compare("my") == 0) continue;
		if(!isMatch(obj->adjectives, c)) return false;
	}
	return true;
};

bool nameMatch(string input) {
	return false;
};

void resolveTarget(vector<string> in) {
	in.erase(in.begin());
	// 		if(in.size() > 0 && (in[0].compare("at") == 0 || in[0].size() != 0)) cid = COMMAND_LOOK_AT;
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;

	// check to ensure there is a command to be parsed
	if (in.size() > 0) {

		// grab target and remove it from vector
		string targ = in.at(in.size() - 1);
		in.erase(in.end()-1);

		// search player items
		for (int i = 0; i < (int)ip->size(); i++) {
			if (isMatch(ip->at(i).name, targ)) {
				// adjective check
				if(adjCheck(in, &ip->at(i))) {
					// c-style - could not find an alternative
					target.ptype = TYPE_PLAYER;
					target.parent = reinterpret_cast<void *>(&p);
					target.type = TYPE_ITEM;
					target.target = reinterpret_cast<void *>(&ip->at(i));
				}
			}
		}

		// search room items
		for (int i = 0; i < (int)ir->size(); i++) {
			if (isMatch(ir->at(i).name, targ)) {
				// adjective check
				if (adjCheck(in, &ir->at(i))) {
					// c-style - could not find an alternative
					target.ptype = TYPE_ROOM;
					target.parent = reinterpret_cast<void *>(&r);
					target.type = TYPE_ITEM;
					target.target = reinterpret_cast<void *>(&ir->at(i));
				}
			}
		}

		// search user weapon
		if (isMatch(p->weapon.name, targ)) {
			if (adjCheck(in, &p->weapon)) {
				// c-style - could not find an alternative
				target.ptype = TYPE_PLAYER;
				target.parent = reinterpret_cast<void *>(&p);
				target.type = TYPE_ITEM;
				target.target = reinterpret_cast<void *>(&p->weapon);
			}
		}
		
		if (isMatch(m->name, targ)) {
			if (adjCheck(in, m)) {
				// c-style - could not find an alternative
				target.ptype = TYPE_ROOM;
				target.parent = reinterpret_cast<void *>(&r);
				target.type = TYPE_MONSTER;
				target.target = reinterpret_cast<void *>(m);
			}
		}
		

		//if (isMatch(, target)) cout << "Affirmitive match: " << m->name << endl;
		// search player if (isMatch(p->name, target)) cout << "Affirmitive match: " << m->name << endl;
	}
};

int determineAction(vector<string> in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	string cmd, adjectives;
	int cid;
	// identify command
	cmd = in[0];
	if (isMatch("fight", cmd)) {
		cid = COMMAND_FIGHT;
	} else if (isMatch("run", cmd)) {
		cid = COMMAND_RUN;
	} else if (isMatch("continue", cmd)) {
		cid = COMMAND_CONTINUE;
	} else if (isMatch("go", cmd)) {
		cid = COMMAND_GO;
	} else if (isMatch("escape", cmd)) {
		cid = COMMAND_ESCAPE;
	} else if (isMatch("dodge", cmd)) {
		cid = COMMAND_DODGE;
	} else if (isMatch("look", cmd)) {
		cid = COMMAND_LOOK;
	} else if (isMatch("take", cmd)) {
		cid = COMMAND_TAKE;
	} else if(cmd.compare("") == 0) {
		cid = COMMAND_ENTER;
	} else {
		cid = COMMAND_UNKNOWN;
	}
	target.cmd = cid;
	return cid;
};

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
		int pRoll, pHit;
		pRoll = d20();
		pHit = getDamage(p);
		int mRoll, mHit;
		mRoll = d20();
		mHit = getDamage(m);

		cout << "You rolled: " << pRoll << " +" << pHit << endl;
		cout << getName(m) << " rolled: " << mRoll << " +" << mHit << endl;
		pHit += pRoll;
		mHit += mRoll;
		if(pHit >= mHit) {
			cout << "You out-fox the " + getName(m) + " and land a solid blow with your " << getWeapon(p) << "!" << endl;
			m->HP--;
			if(m->HP <= 0) monsterDeath(m);
		} else if(mHit > pHit) {
			cout << "The " + getName(m) + " slips into flanking position and lands a mighty blow with its " << getWeapon(m) << " that sends you reeling!" << endl;
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

void doGo() {
	game_room *r = &rooms[player->rloc];
	game_player *p = player;
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if (target.type == TYPE_ITEM) {
		game_item *ni = reinterpret_cast<game_item *> (target.target);
		if(isExit(ni)) {
				if(isLiving(m)) {
					cout << "The " << getName(m) << " blocks your path!" << endl;
				} else if (isActive(t)) {
					cout << "But the " << getName(t) << " is armed!" << endl;
				} else {
					cout << "You pass through the " << getName(ni) << endl;
					p->rloc = ni->prop1;
					doLookBrief();
				}
		} else {
			cout << "You can't go there!" << endl;
		}
	} else if (target.type == TYPE_MONSTER) {
		game_monster *nm = reinterpret_cast<game_monster *> (target.target);
		cout << "Go through a monster?" << endl;
	} else {
		cout << "Go where?" << endl;
	}
}

// doContinue - CONTINUE command, which moves the player forward
void doContinue() {
	game_room *r = &rooms[player->rloc];
	game_player *p = player;
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if(isOpeningRoom(p->rloc)) {
		cout << "There's no time for walking... RUN!" << endl;
	} else if(p->rloc == CORRIDOR) {
		cout << "There is no obvious path forward." << endl;
	} else if(isLiving(m)) {
		cout << "You can't leave until you've vanquished the " + r->monster.name + "!";
	} else if(t->isArmed) {
		cout << "The room is trapped! Are you trying to die?";
	} else {
		r->beenVisited = true;
		cout << r->transition << endl;
		if(p->rloc == RED_EXIT || p->rloc == GREEN_EXIT || p->rloc == BLUE_EXIT || p->rloc == YELLOW_EXIT) p->rloc = CORRIDOR;
		else p->rloc++;
		r = &rooms[player->rloc];
		m = &r->monster;
		cout << "You continue on into the next area..." << endl;
		doLookBrief();
	}
};

void doLookBrief() {
	game_room *r = &rooms[player->rloc];
	game_player *p = player;
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;

	cout << r->name << endl;
	cout << r->description << endl;
	if (isLiving(m)) {
		cout << "You spot a " << m->name << "!" << endl;
	} if (t->isArmed) {
		cout << "You spot some traps!" << endl;
	} if (r->items.size() == 1) {
		cout << "You spot an item of interest here." << endl;
	} else if (r->items.size() > 1) {
		cout << "You spot several items of interest here." << endl;
	}
}

void doRun() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if (isOpeningRoom(p->rloc)) {
		cout << r->transition << endl;
		p->rloc++;
		r = &rooms[p->rloc];
		cout << r->name << endl;
	} else if(p->rloc == CORRIDOR) {
	
	} else {
		doContinue();
	}
}

// doLook - LOOK command, which shows the player what's happening in the room
void doLook() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	vector<game_item> *ri = &r->items;
	//vector<game_item> *pi = &p->items;
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if(target.type != TYPE_NONE) {
		if(target.type == TYPE_ITEM) {
			game_item *ni = reinterpret_cast<game_item *> (target.target);
			cout << getDescription(ni) << endl;
		} else if (target.type == TYPE_MONSTER) {
			game_monster *nm = reinterpret_cast<game_monster *> (target.target);
			cout << getDescription(nm) << endl;
		} else {
			// needs to be fixed later - doesn't work
			cout << "Look at what?" << endl;
			//cout << getDescription(reinterpret_cast<game_item *> (target.target)) << endl;
		}
	} else {
		if (isOpeningRoom(p->rloc)) {
			cout << "It's all a blur.... keep RUNNING!" << endl;
		} else {
			cout << r->name << endl;
			cout << r->description << endl;
			if (isLiving(m)) {
				cout << "A " << getDescriber(m) << " stands before you." << endl;
			} else if(!isLiving(m) && m->difficulty != MONSTER_NONE) {
				cout << "The corpse of a " << m->name << " lies motionless here." << endl;
			}
			if(ri->size() > 0) {
				cout << "You also see: " << endl;
				for(game_item gi : *ri) {
					cout << "a " << getDescriber(&gi) << endl;
				}
				/*for(int i=0; i < (int)ri->size(); i++) {
					game_item *gi = &ri->at(i);
					cout << "a " << getDescriber(gi) << endl;
				}*/
			}

			if (t->isArmed) {
				cout << "You spot some traps!" << endl;
			}
		}
	}
}

void doUse() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
}

void doTake() {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	if (target.type != TYPE_NONE) {
		if (target.type == TYPE_ITEM) {
			game_item *ni = reinterpret_cast<game_item *> (target.target);
			if(!checkB(ni->flags, BIT_IMMOBILE)) {
				// item can be taken
				cout << target.ptype << endl << endl;
				if(target.ptype == TYPE_ROOM) {
					// alternative code for OO
					//vector<game_item> *ri = &(reinterpret_cast<game_room *> (target.parent))->items;
					for(int i=0; i < (int)ir->size(); i++) {
						// match target with inventory
						if(areEqual(&ir->at(i), ni)) {
							cout << "You pick up the " << ni->name << endl;
							// add to inventory
							ip->push_back(*ni);
							// erase from old item list
							ir->erase(ir->begin() + i);
						}
					}

				} else if(target.ptype == TYPE_PLAYER) {
					cout << "You're already carrying that." << endl;
				} else {
					cout << "doTake() error: How did we arrive here?" << endl;
				}
			} else {
				// immobile
				cout << "You can't carry that!" << endl;
			}
		} else if (target.type == TYPE_MONSTER) {
			game_monster *nm = reinterpret_cast<game_monster *> (target.target);
			cout << "I think the " << getName(m) << " might have a problem with that." << endl;
		} else {
			// needs to be fixed later - doesn't work
			cout << "Take what?" << endl;
			//cout << getDescription(reinterpret_cast<game_item *> (target.target)) << endl;
		}
	}
}


vector<string> getInput() {
	string input;
	getline(cin, input);
	cin.seekg(ios::end);
	cin.clear();
	//cin.ignore(INT_MAX, '\0');
	return delimit(input, ' ');
};

vector<string> delimit(const string &toSplit, char delim) {
	vector<string> result;
	stringstream stringStream (toSplit);
	string item;
	while (getline(stringStream, item, delim)) result.push_back(item);
	if(result.size() < 1) result.push_back("");
	return result;
}

// trimFirst - trim off string according to delimiter
string trimFirst(string &toTrim, char delim) {
	stringstream stringStream(toTrim);
	string item;

	// split string by delim and remove it from source
	getline(stringStream, item, delim);
	toTrim.replace(0, item.length(), "");

	return item;
}

bool isMatch(string src, string text) {
	if(text.length() == 0) return false;
	vector<string> trimmed = delimit(src, ' ');
	for(string s : trimmed) {
		int num = s.find(text, 0);
		if(num != -1 && num == 0) return true;
	}
	return false;
};

bool isNeutralRoom(int r) {
	if (r == FOREST || r == FOREST_2 || r == FOREST_3 || r == PIT_1 || r == PIT_2 || r == ENTRY || r == CORRIDOR || \
		r == RED_EXIT || r == GREEN_EXIT || r == BLUE_EXIT || r == YELLOW_EXIT || \
		r == BOSS_ENTRY || r == EXIT) return true;
	return false;
}

bool isOpeningRoom(int r) {
	if (r == FOREST || r == FOREST_2 || r == FOREST_3 || r == PIT_1 || r == PIT_2) return true;
	return false;
}


void monsterDeath(game_monster *m) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	vector<game_item> *ir = &r->items;
	vector<game_item> *im = &m->items;
	cout << "The " + m->name + " crumbles to the ground lifelessly." << endl;
	if(im->size() > 0) cout << "His items fall to the ground!" << endl;
	for(game_item gi : *im) {
		ir->push_back(gi);
	}
	if(m->weapon.type != ITEM_FIST) {
		ir->push_back(m->weapon);
		m->weapon.clear();
	}
	im->clear();
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
			m.adjectives = "small green";
			break;
		case MONSTER_RED_GOBLIN:
			m.name = "goblin";
			m.adjectives = "red";
			break;
		case MONSTER_PURPLE_GOBLIN:
			m.name = "goblin";
			m.adjectives = "large purple";
			break;
		case MONSTER_SMALL_SKELETON:
			m.name = "skeleton";
			m.adjectives = "small";
			break;
		case MONSTER_KOBOLD:
			m.name = "kobold";
			m.adjectives = "vicious";
			break;
		case MONSTER_SKELETON:
			m.name = "skeleton";
			m.adjectives = "bleached";
			break;
		case MONSTER_GNOLL:
			m.name = "gnoll";
			m.adjectives = "large";
			break;
		case MONSTER_ORC:
			m.name = "orc";
			m.adjectives = "nasty";
			break;
		case MONSTER_LARGE_SKELETON:
			m.name = "skeleton";
			m.adjectives = "giant";
			break;
		case MONSTER_BUGBEAR:
			m.name = "bugbear";
			m.adjectives = "greasy-looking";
			break;
		case MONSTER_OGRE:
			m.name = "ogre";
			m.adjectives = "massive";
			break;
		default:
			m.name = "error";
			m.adjectives = "error";
			cout << "Unknown critter type: " << newMob << endl;
		break;
	}

	while (rand() % 2) {
		switch (rand() % 3) {
			case 0:
				// generate weapon
				m.items.push_back(createItem((rand() % (ITEM_WEAPON_END - ITEM_WEAPON-1)) + ITEM_WEAPON+1));
				cout << endl;
				break;
			case 1:
				cout << "Armor isn't set up yet." << endl;

				break;
			case 2:
				cout << "Items aren't set up yet." << endl;
				break;
		}
	}

	return m;
};

game_item createItem(int id) {
	game_item newItem;
	switch(id) {
		case ITEM_FIST:
			newItem.name = "fists";
			newItem.adjectives = "";
			newItem.description = "Your old chums, law and order. They haven't let you down yet.";
			newItem.type = id;
			setB(newItem.flags, BIT_WEAPON);
		break;
		case ITEM_CLAW:
			newItem.name = "claws";
			newItem.adjectives = "sharp";
			newItem.description = "Enhanced fingernails. The pointy-ends face out.";
			newItem.type = id;
			setB(newItem.flags, BIT_WEAPON);
			break;
		case ITEM_RED_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large red steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT);
			setB(newItem.flags, BIT_IMMOBILE);
			newItem.prop1 = RED_1;
			break;
		case ITEM_GREEN_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large green steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT);
			setB(newItem.flags, BIT_IMMOBILE);
			newItem.prop1 = BLUE_1;
			break;
		case ITEM_BLUE_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large blue steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT);
			setB(newItem.flags, BIT_IMMOBILE);
			newItem.prop1 = BLUE_1;
			break;
		case ITEM_YELLOW_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large yellow steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT);
			setB(newItem.flags, BIT_IMMOBILE);
			newItem.prop1 = YELLOW_1;
			break;
		case ITEM_GOLD_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large gold steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			newItem.prop1 = BOSS_ENTRY;
			setB(newItem.flags, BIT_EXIT);
			setB(newItem.flags, BIT_IMMOBILE);
			break;
		case ITEM_HOLE:
			newItem.name = "burrow";
			newItem.adjectives = "small rounded";
			newItem.description = "A small burrow is dug into the wall here, though by the markings, you aren't sure \
what dug it. If you were to squeeze, you could probably make your way through it.";
			setB(newItem.flags, BIT_EXIT);
			setB(newItem.flags, BIT_IMMOBILE);
			newItem.prop1 = CORRIDOR;
			break;
		case ITEM_DAGGER:
			newItem.name = "dagger";
			newItem.adjectives = "small";
			newItem.description = "A small dagger.";
			setB(newItem.flags, BIT_WEAPON);
			newItem.prop1 = 2;
			break;
		case ITEM_SCIMITAR:
			newItem.name = "dagger";
			newItem.adjectives = "small";
			newItem.description = "A small dagger.";
			setB(newItem.flags, BIT_WEAPON);
			newItem.prop1 = 3;
			break;
		case ITEM_RAPIER:
			newItem.name = "rapier";
			newItem.adjectives = "rusty";
			newItem.description = "An old rusty rapier.";
			setB(newItem.flags, BIT_WEAPON);
			newItem.prop1 = 4;
			break;
		case ITEM_LONGSWORD:
			newItem.name = "longsword";
			newItem.adjectives = "shiny";
			newItem.description = "A shiny longsword. It looks like it was recently polished.";
			setB(newItem.flags, BIT_WEAPON);
			newItem.prop1 = 5;
			break;
		case ITEM_BROADSWORD:
			newItem.name = "broadsword";
			newItem.adjectives = "weighted";
			newItem.description = "A weighted broadsword, designed to maximize the force of a swing \
without upsetting the balance.";
			setB(newItem.flags, BIT_WEAPON);
			newItem.prop1 = 7;
			break;
		case ITEM_2HSWORD:
			newItem.name = "sword";
			newItem.adjectives = "two-handed";
			newItem.description = "A massive two-handed sword. Anyone hit by this thing is going to be \
in some serious pain";
			setB(newItem.flags, BIT_WEAPON);
			newItem.prop1 = 9;
		break;
		default:
			cout << "createItem(): unknown id (" << id << ")" << endl;
		break;
	}
	newItem.type = id;
	return newItem;
};



// d20 function - returns a roll from 1 to 20, +/- bias
int d20(int bias) {
	int roll = rand() % 20 + 1;
	roll += bias;
	(roll > 20 ? roll = 20 : 0);
	return roll;
}

/*
//Function to display High Scores
void hiScores() {
	ifstream inFile;
	inFile.open("High Scores.txt");
	if (inFile) {
		cout << "The High Scores are:" << endl;
		for (int counter = 0; counter < 5; counter++) {
			inFile >> hiScore[counter];
			cout << hiScore[counter] << endl;
		}
	} else
		cout << "There are no High Scores yet. Please play the game to add a High Score." << endl;
	inFile.close();
}


//Function to write new high scores to disk.
void newHiScores() {
	ofstream outFile;
	outFile.open("High Scores.txt");
	if (player->gold > hiScore[0]) {
		hiScore[4] = hiScore[3];
		hiScore[3] = hiScore[2];
		hiScore[2] = hiScore[1];
		hiScore[1] = hiScore[0];
		hiScore[0] = player->gold;
	} else if (player->gold > hiScore[1]) {
		hiScore[4] = hiScore[3];
		hiScore[3] = hiScore[2];
		hiScore[2] = hiScore[1];
		hiScore[1] = player->gold;
	} else if (player->gold > hiScore[2]) {
		hiScore[4] = hiScore[3];
		hiScore[3] = hiScore[2];
		hiScore[2] = player->gold;
	} else if (player->gold > hiScore[3]) {
		hiScore[4] = hiScore[3];
		hiScore[3] = player->gold;
	} else if (player->gold > hiScore[4])
		hiScore[4] = player->gold;

	for (int counter = 0; counter < 5; counter++) {
		outFile << hiScore[counter];
	}
	outFile.close();
}*/


void initRooms() {
	rooms = new game_room[GAME_ROOM_SIZE];
	for (int i = 0; i < GAME_ROOM_SIZE; i++) {
		game_room *r = &rooms[i];
		game_monster *m = &r->monster;
		game_trap *t = &rooms[i].trap;
		if (!isNeutralRoom(i)) {
			t->isArmed = rand() % 2;
			r->hasRelic = rand() % 2;
			if (rand() % 2) {
				*m = createMonster(MONSTER_RANDOM);
			}
		}
	}

	// neutral rooms
	game_room *r = &rooms[FOREST];
	r->name = "Running through the forest";
	r->description = "In the distance, you hear a faint siren echoing through the trees. \
The trolls are after you, and you can hear their footsteps getting louder. You barely managed to slip \
out unscathed after they knocked your door down, but they were in fast pursuit. As you made your escape, \
they had already slain the chief and seemed to be rounding people up - anyone who tried to run was killed, \
but you were the lucky one and got a head-start. Their fates (and your own) rest on that your feet rumble \
faster than the troll's bellies.";
	r->transition = "You continue your sprint, trying to outpace the trolls in pursuit. The branches \
seem to unfurl towards you like outstretched fingers, frantically clawing at your face and clothes - much like \
your pursuers, if they catch you. One thought dominates all, \'Don't stop: just keep moving.\'";
	setB(r->flags, BIT_RUNNING);

	r = &rooms[FOREST_2];
	r->transition = "As you bustle through the dark forest, it dawns on you that you no longer \
have any idea where you are - you've never been this deep before, let alone in complete darkness. That doesn't \
stop your running though - trolls are much scarier than darkness. Suddenly, with one errant step, the ground \
exits beneath you, and you find yourself tumbling into the unknown.";

	r = &rooms[FOREST_3];
	r->transition = "You continue your nascent plunge, devoid of rhyme or reason. After jarring \
yourself several times against the mysteries of the night, you finally hit ground with a dull thud, knocking \
the breath from your chest. You take a second to recover, only to realize that you there is no way out of this \
pit as it dawns on you may have fallen into a trap. You feel around until you locate a gap in the darkness and \
frantically begin your descent, hopefully out of danger \
for a change?";

	r = &rooms[PIT_1];
	r->transition = "As you continue into the hole, you catch a glimpse of light in the distance, \
but give pause when you realize that there is no natural light underground. You slow your stride and quietly move \
towards the light, when at once, you jerk yourself to a stop as you notice a large chasm taking shape shape \
between you and your destination. It seems your only hope is to try to jump it";

	r = &rooms[PIT_2];
	r->transition = "Gathering up your courage, you jump.";

	r = &rooms[ENTRY];
	r->description = "You jumped and are now in front of a large metal door. There's a hole you can crawl \
through";
	r->transition = "You crawl down into the hole and begin shimmying through. Loose dirt flies up in \
your hair and on your face and clothes, and the hole seems to narrower. With significant labor, you squeeze through";
	r->items.push_back(createItem(ITEM_HOLE));

	r = &rooms[CORRIDOR];
	r->name = "Corridor, Chamber";
	"You scan the pit and see an opening on the wall.You approach it.As you \
approach the opening you notice that torches line the walls lighting the room you've just walked into. You open the door and head down the dimly lit passage. After a short while, the passage opens into a room.";
	r->description = "It's dim, but you can see\
that there are five doors on the wall opposite you. As you cautiously walk closer to the doors, you hear a loud rumble, as if \
the earth itself is being split in two. Without warning the entrance behind you caves in. You're trapped. Your only choice is to\
go through one of the doors ahead of you. As you move closer, you can make out more details about the doors. On the left there are\
two doors, one red and one green. On the right there are two more, one blue and one yellow. In the center there is a massive, ornately\
decorated door that appears to be made of solid gold.";
	r->items.push_back(createItem(ITEM_BLUE_DOOR));
	r->items.push_back(createItem(ITEM_GREEN_DOOR));
	r->items.push_back(createItem(ITEM_RED_DOOR));
	r->items.push_back(createItem(ITEM_YELLOW_DOOR));
	r->items.push_back(createItem(ITEM_GOLD_DOOR));
	r->items.push_back(createItem(ITEM_DAGGER));

	//If this description needs to be less detailed in order to reuse the room later in the program, we can go with something like, You enter
	//the main room of the cave system. Opposite you are 5 doors, one red, one green, one blue and one yellow. In the center is a massive
	//golden door. Which door will you go through?

	r = &rooms[RED_EXIT];
	r->name = "Red Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	// transition text: ejected back into corridor

	r = &rooms[GREEN_EXIT];
	r->name = "Green Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";

	r = &rooms[BLUE_EXIT];
	r->name = "Blue Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";

	r = &rooms[YELLOW_EXIT];
	r->name = "Yellow Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";

	// populated rooms - TODO
	r = &rooms[RED_1];
	r->name = "Small Room";
	r->description = "You come into a room that is fairly small and nondescript - the smell of stale air pierces your nostrils. It is \
lit by a few torches on the wall.";

	r = &rooms[RED_2];
	r->name = "Small Pantry";
	r->description = "You enter the room, and you can tell that it is, or was, a pantry. There are cupboards along the walls and what\
	appears to be several preparation tables in the center of the room.";

	r = &rooms[RED_3];
	r->name = "Large Sleeping Quarters";
	r->description = "When you enter this room you see several rows of bunk beds that stretch off into the distance, fading into the \
	dark. As far as you can tell, there is nothing, or no one, sleeping in the beds.";

	r = &rooms[RED_4];
	r->name = "Medium Library";
	r->description = "As you cross the threshhold of this room, you see rows of bookcases all filled to the brim with books. This is \
	one of the brightest rooms you've been in so far, thanks to the chandeliers hanging from the ceiling.";

	r = &rooms[RED_5];
	r->name = "Large Forge";
	r->description = "As you enter the room, the first thing you notice is that it is very hot. You see the forge and realize that must\
	be where the heat is coming from. There are workbenches all over the room with tools scattered haphazardly across them.";

	r = &rooms[GREEN_1];
	r->name = "Small Storage Room";
	r->description = "This room is another small storage room. It has several cabinets in it, as well as a stack of crates in the corner";

	r = &rooms[GREEN_2];
	r->name = "Medium Kitchen";
	r->description = "This room has several prep tables as well as a sink to wash dishes, and several ovens. You think of the last meal \
	that you had. Beef stew. Your stomach rumbles and you're reminded of your hunger.";

	r = &rooms[GREEN_3];
	r->name = "Medium Distillery";
	r->description = "As you enter the room, you see the remnants of several stills along the edges of the room. In the center is a table\
	with several empty bottles scattered across the top.";

	r = &rooms[GREEN_4];
	rooms[GREEN_4].name = "Large Training Room";
	rooms[GREEN_4].description = "Inside this room you see racks of practice weapons as well as several training dummies, two archery targets and a \
	first aid kit.";

	r = &rooms[GREEN_5];
	rooms[GREEN_5].name = "Large Mage Workshop";
	rooms[GREEN_5].description = "As you enter this room you see several arcane tables set up. One of the tables has several crystals on it. One of \
	the crystals is glowing faintly, but you decide it's best if you leave it alone.?";

	r = &rooms[BLUE_1];
	rooms[BLUE_1].name = "Medium Indoor Garden";
	rooms[BLUE_1].description = "As you come into this room, the first thing that hits you is the smell. There is the overwhelming smell of manure. As\
	you cover your nose and mouth to keep from gagging, you see that all manner of moss, lichens, and mushrooms are growing in planter boxes filled \
	with what you can only guess is manure.";

	r = &rooms[BLUE_2];
	rooms[BLUE_2].name = "Large Library";
	rooms[BLUE_2].description = "In this room there are hundreds of bookcases teeming with books. As you glance around you see several that appear to be\
	bound in solid gold. They are beautiful but you decide to keep moving.";

	r = &rooms[BLUE_3];
	rooms[BLUE_3].name = "Burnt Mage Workshop";
	rooms[BLUE_3].description = "Once in this room you can see that it is a mage's workshop. Or it was. It seems as though something went horribly wrong\
	when it was last used. Everything inside is burnt to a crisp, and in the center of the room are the splintered remains of an arcane table.";

	r = &rooms[BLUE_4];
	rooms[BLUE_4].name = "Large Pantry";
	rooms[BLUE_4].description = "You emerge into a large pantry with al sorts of dried meats and cheeses hanging from pegs in the rafters. Despite your \
	hunger, you know that you should not stop and eat.";

	r = &rooms[BLUE_5];
	rooms[BLUE_5].name = "Large Archery Range";
	rooms[BLUE_5].description = "Upon entering this room, you see that it is a large indoor archery range. There is a row of archery targets that stretches\
	off into the depths of the darkness.";

	r = &rooms[YELLOW_1];
	rooms[YELLOW_1].name = "Large Tavern";
	rooms[YELLOW_1].description = "You come into this room and see that it is a tavern. There are several tables set up, with chairs, as well as a row of bar\
	stools set up along the bar. You see the tap handles behind the bar and you wish that you had the time to enjoy a pint.";

	r = &rooms[YELLOW_2];
	rooms[YELLOW_2].name = "Large Armory";
	rooms[YELLOW_2].description = "Once you are in this room, you see racks of weapons lined up in the room. All manner of weapons are stored in the racks,\
	swords, maces, warhammers, spears, and many more. In the center of the room is a row of many workbenches littered with an assortment of hammers, tongs\
	whetstones and other various tools.";

	r = &rooms[YELLOW_3];
	rooms[YELLOW_3].name = "Large Dining Hall";
	rooms[YELLOW_3].description = "In this room you see rows of tables lined up stretching into the distance. There are plates at the tables and a candleabra\
	on each table.";

	r = &rooms[YELLOW_4];
	r->name = "Medium Latrine";
	r->description = "You enter this room and see that there are several stalls with toilets on one side, and on the other there are tubs lined up\
	for bathing. You are surprised that it doesn't smell worse than it does.";

	r = &rooms[YELLOW_5];
	r->name = "Large Stable";
	r->description = "Upon entering this room, you realize you've stumbled across a stable. There are several horses in the stalls one side of the\
	room and on the other there are several large bales of hay. You are thankful for the signs of life the horses bring you.";

	r = &rooms[BOSS_ENTRY];
	r->name = "Large Golden Door";
	r->description = "You approach the large golden door with the four keys. As you unlock each lock, one by one, they fall to the ground. As the\
	final lock hits the ground you hear a rumbling and the room starts to shake slightly. The massive doors swing open and there is a blazinf light on the other\
	side of the doors.";

	r = &rooms[BOSS];
	r->name = "Large Throne Room";
	r->description = "As you step through the doors, you find yourself in a brightly lit throne room, with tall vaulted ceilings and many paintings on the \
	\nthe wall. There is a roaring fire in the fireplace on one wall, and across from you you see a massive golden throne.";

	r = &rooms[EXIT];
	r->name = "Dungeon Exit";
	r->description = "As you exit the caves, the sunlight hits your face causing you to squint in the brightness momentarily. You realize that you've been in\
	\n the caves all night. You breathe a sigh of relief as you realize that it's over and you wander off into the forest in search of ";
	// populated rooms - TODO
};