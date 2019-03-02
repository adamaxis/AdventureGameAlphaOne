// AdventureGameAlphaOne.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;


const char *FILE_HISCORES = "HighScores.txt";
const int MAX_SCORES = 5;
int hiScore[MAX_SCORES+1];
void newHiScores();
void readHiScores();


inline void setB(long &src, long val) {
	src |= (val);
};

inline void clearB(long &src, long val) {
	src &= ~(val);
};

inline void toggleB(long &src, long val) {
	src ^= val;
};

inline bool checkB(long &src, long val) {
	return ((src) & (val));
};

enum item_states {
	BIT_IMMOBILE = 1,
	BIT_EXIT = 2,
	BIT_BLOCKED_VISIBLE = 4,
	BIT_LOCKED = 8,
	BIT_KEY = 16,
	BIT_CONTAINER = 32,
	BIT_POTION = 64,
	BIT_WEAPON = 128,
	BIT_ARMOR = 256,
};

enum room_states {
	ROOM_DARK = 1,
	ROOM_RUNNING = 2,
	ROOM_NEUTRAL = 4,
	ROOM_NOEXIT = 8,
	ROOM_JUMPEXIT = 16,
	ROOM_WIN = 32
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
	COMMAND_INVENTORY,
	COMMAND_LOOK,
	COMMAND_USE,
	COMMAND_DODGE,
	COMMAND_JUMP,
	COMMAND_TAKE,
	COMMAND_RUN,
	COMMAND_ESCAPE,
	COMMAND_ENTER,
	COMMAND_WEAR,
	COMMAND_WIELD
};

// states for different types of objects
enum game_object_types {
	TYPE_NONE = 0,
	TYPE_ITEM,
	TYPE_MONSTER,
	TYPE_PLAYER,
	TYPE_ROOM,
	TYPE_UNKNOWN
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
	EXIT,
	SECRET_CHAMBER,
	END
};

enum game_traps {
	TRAP_NONE = 0,
	TRAP_NORMAL
};

enum game_items {
	ITEM_FISTS = -10,
	ITEM_CLAWS,
	ITEM_CLOTHES,
	ITEM_RAGS,
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
	ITEM_BURROW_ENT,
	ITEM_BURROW_EXIT,
	ITEM_SECRET_ENT,
	ITEM_SECRET_EXIT,
	ITEM_WEAPON=100,			// weapon drops begin
	ITEM_DAGGER,
	ITEM_SCIMITAR,
	ITEM_RAPIER,
	ITEM_LONGSWORD,
	ITEM_BROADSWORD,
	ITEM_2H_SWORD,
	ITEM_WEAPON_END,			// weapon drops end
	ITEM_ARMOR=200,				// armor drops begin
	ITEM_CLOTH_ROBE,
	ITEM_ROTTING_LEATHER,
	ITEM_STUDDED_LEATHER,
	ITEM_CHAIN_SHIRT,
	ITEM_RUSTY_PLATE,
	ITEM_ARMOR_END,				// armor drops end
	ITEM_ITEM = 300,			// item drops begin
	ITEM_SMALL_POTION,
	ITEM_LARGE_POTION,
	ITEM_ELIXIR,
	ITEM_ITEM_END,				// item drops end
	ITEM_BOTTLE,
	ITEM_SHINY_PLATE,
	ITEM_GOLD_KEY,
	ITEM_RANDOM_TREASURE_BOX
};


struct game_item {
	string name;
	string adjectives;
	string description;
	int type = ITEM_NONE;
	long flags = 0;
	long prop1 = 0;
	long prop2 = 0;
	string sprop1, sprop2;
	void *vprop1;
	int id;

	// isExit - prop1 contains exit ID, sporp1 contains exit text
	// isLocked - prop2 contains key ID
	// isArmor - prop1 contains armor def
	// isWeapon - prop1 contains weapon dmg
	// isContainer - vprop1 contains object list

	game_item() {
		id = rand();
	}

	void clear() {
		name = adjectives = description = sprop1 = sprop2 = "";
		type = ITEM_NONE;
		flags = prop1 = prop2 = 0;
	}

	void copy(game_item c) {
		name = c.name;
		adjectives = c.adjectives;
		description = c.description;
		type = c.type;
		flags = c.flags;
		prop1 = c.prop1;
		prop2 = c.prop2;
		sprop1 = c.sprop1;
		sprop2 = c.sprop2;
		id = c.id;
	}

};

game_item createItem(int);


// function prototypes
vector<string> delimit(const string &, char);
string trimFirst(string &, char);
bool isMatch(string, string);

int determineAction(vector<string> &);
void resolveTarget(vector<string> &);
vector<string> getInput();
void initRooms();
int d20(int = 0);

// action function prototypes
void doRun(vector<string> &);
void doContinue(vector<string> &);
void doGo(vector<string> &);
void doFight(vector<string> &);
void doDodge(vector<string> &);
void doJump(vector<string> &);
void doLook(vector<string> &);
void doLookBrief(vector<string> &);
void doUse(vector<string> &);
void doInventory(vector<string> &);
void doTake(vector<string> &);
void doWield(vector<string> &);
void doWear(vector<string> &);

void doGameCheck(vector<string> &);


template<class T>
bool isActive(T *t) {
	return t->isArmed;
};


// weapon functions
template<class T>
string getWeapon(T *entity) { return entity->weapon.name; };

template<class T>
int getDamage(T *entity) { return entity->weapon.prop1; };


// armor functions
template<class T>
string getArmor(T *entity) { return entity->armor.name; };

template<class T>
int getDefense(T *entity) { return entity->armor.prop1; };

// potion functions
template<class T>
int getHealAmt(T *entity) { return entity->prop1; };



// generic object functions
template<class T>
bool areEqual(T *entity, T *entity2) { return (entity->id == entity2->id); };

template<class T>
bool isLiving(T & entity) { return (entity->HP > 0); };

template<class T>
string getDescriber(T *entity) { return (entity->adjectives + " " + entity->name); };

template<class T>
string getName(T & entity) { return (entity->name); };

template<class T>
string getDescription(T & entity) { return (entity->description); };




// item bit checkers
template<class T>
bool isWeapon(T *i) { return checkB(i->flags, BIT_WEAPON); };

template<class T>
bool isArmor(T *i) { return checkB(i->flags, BIT_ARMOR); };

template<class T>
bool isPotion(T *i) { return checkB(i->flags, BIT_POTION); };

template<class T>
bool isContainer(T *i) { return checkB(i->flags, BIT_CONTAINER); };

template<class T>
bool isExit(T *i) { return checkB(i->flags, BIT_EXIT); };

// room bit checkers
template<class T>
bool hasExit(T *i) { return !checkB(i->flags, ROOM_NOEXIT); };

template<class T>
bool isNeutral(T *r) { return checkB(r->flags, ROOM_NEUTRAL); };

template<class T>
bool isRunning(T *r) { return checkB(r->flags, ROOM_RUNNING); };


// player struct - self explanatory
struct game_player {
public:
	string name;
	int bonus;
	long gold;
	int HP;
	int rloc;
	vector<game_item> items;
	game_item weapon, armor;

	game_player() {
		gold = 0;
		rloc = FOREST;
		weapon = createItem(ITEM_FISTS);
		armor = createItem(ITEM_CLOTHES);
		HP = 3;
	}

};

// monster struct, for fleshing out monsters
struct game_monster {
	string name, description, adjectives;
	int HP;
	game_item weapon, armor;
	vector<game_item> items;
	int difficulty;

	game_monster() {
		difficulty = MONSTER_NONE;
		description = "none";
		HP = 0;
		weapon = createItem(ITEM_CLAWS);
		armor = createItem(ITEM_RAGS);
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
	long prop1, prop2;
	game_trap trap;

	game_room() {
		flags = 0;
		name = description = transition = "";
		hasRelic = beenVisited = false;
	}

	game_room(string name, string description, bool hasRelic) {
		this->name = name;
		this->description = description;
		this->hasRelic = hasRelic;
	}
};



struct game_command {
	int cmd = COMMAND_UNKNOWN;
	int type=0, ptype=0;
	void *target, *parent;

	void clear() {
		cmd = COMMAND_UNKNOWN;
		type = ptype = TYPE_NONE;
		target = nullptr, parent = nullptr;
	}
};

game_command searchByType(long = 0);
game_command searchByString(vector<string> &);


// constant for rooms array
const int GAME_ROOM_SIZE = END+1;

// globals
game_player *player;
game_room *rooms;
game_command target;
vector<void *> containers;

int gameState;

int main()
{
	// initialize randomizer
	srand((int) time(0));

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
						readHiScores();
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
				//cout << rooms[player->rloc].name << endl;
				//cout << rooms[player->rloc].description << endl;
				command.clear();
				doLookBrief(command);
				// game = running
				gameState = STATE_PLAYING;
			break;
			// game is running
			case STATE_PLAYING:
				// clear target
				command = getInput();
				action = determineAction(command);
				resolveTarget(command);
				// identify target
				if (action && isLiving(player)) {
					switch (action) {
						case COMMAND_CONTINUE:
							doContinue(command);
						break;
						case COMMAND_GO:
							doGo(command);
						break;
						case COMMAND_LOOK:
							doLook(command);
						break;
						case COMMAND_FIGHT:
							doFight(command);
						break;
						case COMMAND_DODGE:
							doDodge(command);
						break;
						case COMMAND_RUN:
							doRun(command);
						break;
						case COMMAND_USE:
							doUse(command);
							break;
						case COMMAND_WIELD:
							doWield(command);
							break;
						case COMMAND_WEAR:
							doWear(command);
							break;
						case COMMAND_JUMP:
							doJump(command);
						break;
						case COMMAND_TAKE:
							doTake(command);
						break;
						case COMMAND_INVENTORY:
							doInventory(command);
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
					doGameCheck(command);
				}
			break;
			// player died
			case STATE_GAMEOVER:
				newHiScores();
				// cleanup
				// delete container-list for boxes
				int ci = containers.size();
				for(int i=0; i < ci; i++) {
					// c-style object reference
					vector<game_item> *li = reinterpret_cast<vector<game_item> *> (containers.at(0));
					delete li;
					// erase it from container list as well
					containers.erase(containers.begin());
				}
				// delete rooms and players
				delete[] rooms;
				delete player;
				gameState = STATE_MENU;
			break;
		}
	}
}

void doGameCheck(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	if (!isLiving(p)) {
		// death is handled here
		cout << "You have died!" << endl;
		gameState = STATE_GAMEOVER;
	} else if(checkB(r->flags, ROOM_WIN)) {
		// player reached the final room
		cout << "You have won!" << endl;
		gameState = STATE_GAMEOVER;
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

void resolveTarget(vector<string> &in) {
	target = searchByString(in);
};

game_command searchByType(long type) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	game_command target;
	// check to ensure there is a command to be parsed

	if (type != TYPE_NONE) {
		// search player items
		for (int i = 0; i < (int)ip->size(); i++) {
			if (ip->at(i).type == type) {
				// c-style - could not find an alternative
				target.ptype = TYPE_PLAYER;
				target.parent = reinterpret_cast<void *>(&p);
				target.type = TYPE_ITEM;
				target.target = reinterpret_cast<void *>(&ip->at(i));
				break;
			}
		}

		// search room items
		for (int i = 0; i < (int)ir->size(); i++) {
			if (ir->at(i).type == type) {
				// c-style - could not find an alternative
				target.ptype = TYPE_ROOM;
				target.parent = reinterpret_cast<void *>(&r);
				target.type = TYPE_ITEM;
				target.target = reinterpret_cast<void *>(&ir->at(i));
				break;
			}
		}


	// check user weapon
		if (p->weapon.type == type) {
			// c-style - could not find an alternative
			target.ptype = TYPE_PLAYER;
			target.parent = reinterpret_cast<void *>(&p);
			target.type = TYPE_ITEM;
			target.target = reinterpret_cast<void *>(&p->weapon);
		}

		// check user armor
		if (p->armor.type == type) {
			// c-style - could not find an alternative
			target.ptype = TYPE_PLAYER;
			target.parent = reinterpret_cast<void *>(&p);
			target.type = TYPE_ITEM;
			target.target = reinterpret_cast<void *>(&p->armor);
		}

		if (target.type == TYPE_NONE) target.type = target.ptype = TYPE_UNKNOWN;
	};
	return target;
};

game_command searchByString(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	game_command target;
	// check to ensure there is a command to be parsed

	if (in.size() > 0) {
		// grab target and remove it from vector
		string targ = in.at(in.size() - 1);
		in.erase(in.end() - 1);

		// search player items
		for (int i = 0; i < (int)ip->size(); i++) {
			if (isMatch(ip->at(i).name, targ)) {
				// adjective check
				if (adjCheck(in, &ip->at(i))) {
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

		// check user weapon
		if (isMatch(p->weapon.name, targ)) {
			if (adjCheck(in, &p->weapon)) {
				// c-style - could not find an alternative
				target.ptype = TYPE_PLAYER;
				target.parent = reinterpret_cast<void *>(&p);
				target.type = TYPE_ITEM;
				target.target = reinterpret_cast<void *>(&p->weapon);
			}
		}

		// check user armor
		if (isMatch(p->armor.name, targ)) {
			if (adjCheck(in, &p->armor)) {
				// c-style - could not find an alternative
				target.ptype = TYPE_PLAYER;
				target.parent = reinterpret_cast<void *>(&p);
				target.type = TYPE_ITEM;
				target.target = reinterpret_cast<void *>(&p->armor);
			}
		}

		// check monster
		if (isMatch(m->name, targ)) {
			if (adjCheck(in, m)) {
				// c-style - could not find an alternative
				target.ptype = TYPE_ROOM;
				target.parent = reinterpret_cast<void *>(&r);
				target.type = TYPE_MONSTER;
				target.target = reinterpret_cast<void *>(m);
			}
		}
		in.push_back(targ);
		if (target.type == TYPE_NONE) target.type = target.ptype = TYPE_UNKNOWN;
		// search player if (isMatch(p->name, target)) cout << "Affirmitive match: " << m->name << endl;
	};
	return target;
};

int determineAction(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	string cmd, adjectives;
	int cid;
	// identify command
	cmd = in[0];
	in.erase(in.begin());
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
	} else if (isMatch("inventory", cmd)) {
		cid = COMMAND_INVENTORY;
	} else if (isMatch("use", cmd)) {
		cid = COMMAND_USE;
	} else if (isMatch("wield", cmd)) {
		cid = COMMAND_WIELD;
	} else if (isMatch("wear", cmd)) {
		cid = COMMAND_WEAR;
	} else if (isMatch("jump", cmd)) {
		cid = COMMAND_JUMP;
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
void doFight(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	if (isRunning(r)) {
		cout << "Fight? Fight the urge to fight and just RUN!" << endl;
	} else if (m->difficulty == MONSTER_NONE) {
		cout << "There is nothing to fight!" << endl;
	} else if(!isLiving(m)) {
		cout << "But the " + m->name + " is already dead!" << endl;
	} else {
		int pRoll, pHit;
		pRoll = d20();
		pHit = getDamage(p);
		int mRoll, mHit, pDef, mDef;
		mRoll = d20();
		mHit = getDamage(m);
		pDef = getDefense(p);
		mDef = getDefense(m);


		cout << "You rolled: " << pRoll << " +" << pHit << " -" << mDef << endl;
		cout << getName(m) << " rolled: " << mRoll << " +" << mHit << " -" << pDef << endl;
		pHit += pRoll - mDef;
		mHit += mRoll - pDef;
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
void doDodge(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if (isRunning(r)) {
		cout << "There's no time to look for traps.... just RUN!" << endl;
	} else if (t->isArmed) {
		if(isLiving(m)) {
			cout << "You attempt to dodge through the traps with the " + getName(m) + " in hot pursuit!" << endl;
		} else {
			cout << "You attempt to dodge through the traps!" << endl;
		}
		int pRoll = d20();
		int tRoll = d20();
		int mRoll = d20();
		cout << "You rolled: " << pRoll << endl;
		cout << "Trap rolled: " << tRoll << endl;
		if(isLiving(m)) cout << getName(m) << " rolled: " << mRoll << endl;

		if (pRoll >= tRoll) {
			cout << "You safely manuever through the trap!" << endl;
		} else if (tRoll > pRoll) {
			cout << "You failed to dodge the trap!" << endl;
			player->HP--;
		}
		if(isLiving(m)) {
			if (mRoll > tRoll) {
				cout << "The " << getName(m) << " shucks and shuffles through the trap!" << endl;
			} else if (tRoll >= mRoll) {
				cout << "The " << getName(m) << " rolled the wrong way and got hit by the trap!" << endl;
				m->HP--;
				if (m->HP <= 0) monsterDeath(m);
			}
		}
		t->isArmed = false;

	} else {
		cout << "But there are no traps!" << endl;
	}
};

void doJump(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if(checkB(r->flags, ROOM_JUMPEXIT) && in.size() == 0) {
		cout << "Giving yourself enough room for a running start, you take a flying leap into the unknown!" << endl;
		cout << r->transition;
		p->rloc = r->prop1;
		doLookBrief(in);
	} else {
		game_item *ni = reinterpret_cast<game_item *> (target.target);
		if(target.ptype == TYPE_PLAYER) cout << "Trying to put distance between yourself and your " << ni->name << "?" << endl;
		else if(target.ptype == TYPE_ROOM) cout << "You jump back from the " << ni->name << "!" << endl;
		else if(target.ptype == TYPE_PLAYER) cout << "That seems completely impossible." << endl;
		else if(in.size() == 1 && isMatch("backflip", in[0])) {
			int pRoll = d20();
			cout << "You rolled: " << pRoll << "." << endl;
			if(pRoll >= 5) cout << "You do an incredible backflip!." << endl;
			else cout << "You turn too far and land on your stomach. You hope no one noticed." << endl;
		}
		else cout << "You bounce up and down like a little kid." << endl;
	}
}

// doGo()  - written by John with some additions by Daniel
// GO <exit> - goes through an exit (if it's available).
void doGo(vector<string> &in) {
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
					if(checkB(ni->flags, BIT_LOCKED)) {
						// search inventory for key
						game_command key = searchByType(ni->prop2);
						if(key.type == TYPE_ITEM) {
							if(key.ptype == TYPE_ROOM) {
								cout << "The " << getName(ni) << " seems to be locked" << endl;
							} else if (key.ptype == TYPE_MONSTER) {
								cout << "error: searchByType() has targetted a monster, somehow." << endl;
							} else if (key.ptype == TYPE_PLAYER) {
								game_item *ki = reinterpret_cast<game_item *> (key.target);
								cout << "You unlock the " << getName(ni) << " with your " << getName(ki) << "." << endl;
								clearB(ni->flags, BIT_LOCKED);
							}
						}
					}
					if(checkB(ni->flags, BIT_LOCKED)) {
						cout << "The " << getName(ni) << " seems to be locked" << endl;
					} else {
						cout << "You pass through the " << getName(ni) << endl;
							p->rloc = ni->prop1;
							doLookBrief(in);
					}
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
void doContinue(vector<string> &in) {
	game_room *r = &rooms[player->rloc];
	game_player *p = player;
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if(isRunning(r)) {
		cout << "There's no time for walking... RUN!" << endl;
	} else if(!hasExit(r)) {
		cout << "There is no obvious path forward." << endl;
	} else if(isLiving(m)) {
		cout << "You can't leave until you've vanquished the " + r->monster.name + "!" << endl;
	} else if(t->isArmed) {
		cout << "The room is trapped! Are you trying to die?" << endl;
	} else {
		r->beenVisited = true;
		cout << r->transition << endl;
		if(p->rloc == RED_EXIT || p->rloc == GREEN_EXIT || p->rloc == BLUE_EXIT || p->rloc == YELLOW_EXIT) p->rloc = CORRIDOR;
		else p->rloc++;
		r = &rooms[player->rloc];
		m = &r->monster;
		cout << "You continue on into the next area..." << endl;
		doLookBrief(in);
	}
};

void doLookBrief(vector<string> &in) {
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

void doRun(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	if (isRunning(r)) {
		cout << r->transition << endl;
		p->rloc++;
		r = &rooms[p->rloc];
		cout << r->name << endl;
	} else if(hasExit(r)) {
		cout << "This is where run should work" << endl;
	} else if(!hasExit(r)) {
		cout << "There is no obvious path forward." << endl;
	} else {
		doContinue(in);
	}
}

// doLook - LOOK command, which shows the player what's happening in the room
void doLook(vector<string> &in) {
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
		if (isRunning(r)) {
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
					if(!checkB(gi.flags, BIT_IMMOBILE)) cout << "a " << getDescriber(&gi) << endl;
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

void doInventory(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	vector<game_item> *pi = &p->items;
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	cout << "You are carrying:" << endl;
	for (game_item gi : *pi) {
		cout << "a " << getDescriber(&gi) << "," << endl;
	}
	if(pi->size() == 0) cout << "nothing." << endl;
	else cout << endl;
}

void doUse(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	if (target.type != TYPE_NONE) {
		if (target.type == TYPE_ITEM) {
			game_item *ni = reinterpret_cast<game_item *> (target.target);
			// item can be taken
			if (target.ptype == TYPE_ROOM) {	// the object is on the ground
				if(isExit(ni)) {
					// pretend it's movement
					doGo(in);
					return;
				} else if (isContainer(ni)) {	// treasure box
					vector<game_item> *li = reinterpret_cast<vector<game_item> *> (containers[containers.size() - 1]);
					if(li->size() != 0) {
						cout << "You open the " << getDescriber(ni) << " and loot its contents." << endl;
						cout << "You take:" << endl;
						int lSize = li->size();
						for(int i=0; i < lSize; i++) {
							cout << "a " << getDescriber(&li->at(0)) << endl;
							ip->push_back(li->at(0));
							li->erase(li->begin());
						}
					} else {
						cout << "But the " << getDescriber(ni) << " is empty!" << endl;
					}
				} else {						// object is not an exit so ??
					cout << "You reach out and touch the " << getName(ni) << "." << endl;
				}
			} else if (target.ptype == TYPE_PLAYER) {
				if (areEqual(&p->weapon, ni)) {						// weapon and target are the same
					cout << "You wave your " << getName(ni) << " around like a madman." << endl;
				} else if(areEqual(&p->armor, ni)) {				// armor and target are the same
					cout << "You puff out your chest and strut around, showing off your " << getName(ni) << "." << endl;
				} else {
					for (int i = 0; i < (int)ip->size(); i++) {		// search player iventory
						if (areEqual(&ip->at(i), ni)) {				// found target item-list
							if(isPotion(ni)) {
								cout << "You guzzle down the " << getName(ni) << " and smack your lips! Refreshing!" << endl;
								// time for some flavor text
								if (getHealAmt(ni) == 1) cout << "You feel a bit better." << endl;
								else if (getHealAmt(ni) == 2) cout << "You feel a *LOT* better." << endl;
								else if (getHealAmt(ni) == 3) cout << "You feel like a million bucks, holy cow! Bring on the trolls!" << endl;
								// heal player
								p->HP += getHealAmt(ni);
								// cap extra HP at 5
								if(p->HP > 5) p->HP = 5;
								// create bottle
								cout << "You stow the empty bottle away." << endl;
								ip->push_back(createItem(ITEM_BOTTLE));
								// erase old potion
								ip->erase(ip->begin() + i);
								// re-align ni since vector erasure breaks it
								ni = &ip->at(i);
							}
						}
					}
				}
			} else {
				cout << "doUse() error: How did we arrive here?" << endl;
			}
		} else if (target.type == TYPE_MONSTER) {
			game_monster *nm = reinterpret_cast<game_monster *> (target.target);
			cout << "Now, now - there's no need to be rude to the " << getName(nm) << "." << endl;
		} else {
			cout << "USE what?" << endl;
		}
	}
}

void doWield(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	if (target.type != TYPE_NONE) {
		if (target.type == TYPE_ITEM) {
			game_item *ni = reinterpret_cast<game_item *> (target.target);
			if (!checkB(ni->flags, BIT_IMMOBILE)) {
				// item can be taken
				if (target.ptype == TYPE_ROOM) {
					if (!isWeapon(ni)) cout << "You can't wield that!" << endl;
					else cout << "You'll need to pick that up first." << endl;
				} else if (target.ptype == TYPE_PLAYER) {
					if (isWeapon(ni)) {
						if (areEqual(&p->weapon, ni)) {				// weapon and target are the same
							cout << "But you're already wielding that!" << endl;
						} else {
							for (int i = 0; i < (int)ip->size(); i++) {
								if (areEqual(&ip->at(i), ni)) {				// found target item-list
									game_item *wi = &p->weapon;
									if (wi->type < 0) {
										cout << "Your knuckles were getting sore, anyways." << endl;
									} else {
										// add to inventory
										cout << "You stow away your " << getName(wi) << "." << endl;
										ip->push_back(*wi);
										// fix for push_back() messing up ni's pointer
										ni = &ip->at(i);
										wi->clear();
									}
									cout << "You grip the " << ni->name << " firmly." << endl;
									// assign weapon to ni
									p->weapon = *ni;
									// erase from old item list
									ip->erase(ip->begin() + i);
								}
							}
						}
					} else {
						cout << "You can't wield that! It's not a weapon!" << endl;
					}
				} else {
					cout << "doWield() error: How did we arrive here?" << endl;
				}
			} else {
				// immobile
				cout << "You'd have no idea how to do that." << endl;
			}
		} else if (target.type == TYPE_MONSTER) {
			game_monster *nm = reinterpret_cast<game_monster *> (target.target);
			cout << "I'm sure the " << getName(nm) << " is flattered, but not today." << endl;
		} else {
			cout << "WIELD what?" << endl;
		}
	}
}

void doWear(vector<string> &in) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	game_trap *t = &r->trap;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	if (target.type != TYPE_NONE) {
		if (target.type == TYPE_ITEM) {
			game_item *ni = reinterpret_cast<game_item *> (target.target);
			if (!checkB(ni->flags, BIT_IMMOBILE)) {
				// item can be taken
				if (target.ptype == TYPE_ROOM) {
					if(!isArmor(ni)) cout << "You can't wear that!" << endl;
					else cout << "You'll need to pick that up first." << endl;
				} else if (target.ptype == TYPE_PLAYER) {
					if(isArmor(ni)) {
						if (areEqual(&p->armor, ni)) {				// armor and target are the same
							cout << "But you're already wearing that!" << endl;
						} else {
							for (int i = 0; i < (int)ip->size(); i++) {
								if (areEqual(&ip->at(i), ni)) {				// found target item-list
									game_item *ai = &p->armor;
									if(ai->type < 0) {
										cout << "Feels good to put something protective on!" << endl;
									} else {
										// add to inventory
										cout << "You remove your " << getName(ai) << "." << endl;
										ip->push_back(*ai);
										// fix for push_back() messing up ni's pointer
										ni = &ip->at(i);
										ai->clear();
									}
									cout << "You put on the " << ni->name << endl;
									// might assign armor to ni
									p->armor = *ni;
									// erase from old item list
									ip->erase(ip->begin() + i);
								}
							}
						}
					} else {
						cout << "You can't wear that! It's not armor!" << endl;
					}
				} else {
					cout << "doWear() error: How did we arrive here?" << endl;
				}
			} else {
				// immobile
				cout << "You'd have no idea how to do that." << endl;
			}
		} else if (target.type == TYPE_MONSTER) {
			//game_monster *nm = reinterpret_cast<game_monster *> (target.target);
			cout << "Thinking about taking up tanning? Eh, not today." << endl;
		} else {
			cout << "WEAR what?" << endl;
		}
	}
}

void doTake(vector<string> &in) {
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


void monsterDeath(game_monster *m) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	vector<game_item> *ir = &r->items;
	vector<game_item> *im = &m->items;
	switch((rand() % 4) + 1) {
		case 1:
			cout << "The " + getName(m) + " crumbles to the ground." << endl;
		break;
		case 2:
			cout << "The " + getName(m) + " lets out a ghostly wail, before collapsing with a dull thud." << endl;
		break;
		case 3:
			cout << "With a loud crash, the " + getName(m) + " falls into death's embrace." << endl;
		break;
		case 4:
			cout << "The " + getName(m) + " is sent face-first into the ground." << endl;
		break;
	};

	// check to see if he had a weapon
	if (m->weapon.type > 0) {
		cout << "His " << getWeapon(m) << " fall to the ground!" << endl;
		// drop weapon
		ir->push_back(m->weapon);
		m->weapon.clear();
	}

	// check for loot
	if(im->size() > 0) {
		cout << "Looks like he may have dropped some items." << endl;
		// drop carried items
		for(game_item gi : *im) {
			ir->push_back(gi);
		}
	}

	im->clear();
};

// d20 function - returns a roll from 1 to 20, +/- bias
int d20(int bias) {
	int roll = rand() % 20 + 1;
	roll += bias;
	(roll > 20 ? roll = 20 : 0);
	return roll;
}


// John's work Function to display High Scores
void readHiScores() {
	ifstream inFile;
	inFile.open(FILE_HISCORES);
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
	outFile.open(FILE_HISCORES);
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
		outFile << hiScore[counter] << endl;
	}
	outFile.close();
}

// createMonster - spawns a critter and assigns it stats
game_monster createMonster(int id) {
	game_monster m;
	int newMob;
	if(id == MONSTER_RANDOM) newMob = (rand() % 10) - 5;
	else newMob = id;
	m.difficulty = newMob;
	if(m.difficulty < 0) m.HP = 1;
	else if(m.difficulty < 4) m.HP = 2;
	else if (m.difficulty < 10) m.HP = 3;
	else if (m.difficulty >= 10) m.HP = 5;
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
			m.adjectives = "disgusting";
			break;
		case MONSTER_TROLL_KING:
			m.name = "troll";
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
				m.items.push_back(createItem((rand() % (ITEM_ARMOR_END - ITEM_ARMOR - 1)) + ITEM_ARMOR + 1));
				break;
			case 2:
				m.items.push_back(createItem((rand() % (ITEM_ITEM_END - ITEM_ITEM - 1)) + ITEM_ITEM + 1));
				break;
			default:
				cout << "createMonster() error" << endl;
		}
	}

	return m;
};

game_item createItem(int id) {
	game_item newItem;
	vector<game_item> *treasure;
	vector<game_item> *li;
	switch(id) {
		case ITEM_RANDOM_TREASURE_BOX:
			newItem.name = "box";
			newItem.adjectives = "ornate";
			newItem.description = "A metal box. Maybe there's stuff inside?";
			newItem.type = id;
			treasure = new vector<game_item>;
			// throw in some goodies.
			treasure->push_back(createItem((rand() % (ITEM_WEAPON_END - ITEM_WEAPON - 1)) + ITEM_WEAPON + 1));
			treasure->push_back(createItem((rand() % (ITEM_ARMOR_END - ITEM_ARMOR - 1)) + ITEM_ARMOR + 1));
			treasure->push_back(createItem((rand() % (ITEM_ITEM_END - ITEM_ITEM - 1)) + ITEM_ITEM + 1));
			containers.push_back(treasure);
			newItem.prop1 = containers.size()-1;
			setB(newItem.flags, BIT_CONTAINER | BIT_IMMOBILE);
			break;
		case ITEM_FISTS:
			newItem.name = "fists";
			newItem.adjectives = "";
			newItem.description = "Your old chums, law and order. They haven't let you down yet.";
			newItem.type = id;
			newItem.prop1 = 0;
			setB(newItem.flags, BIT_WEAPON | BIT_IMMOBILE);
		break;
		case ITEM_CLAWS:
			newItem.name = "claws";
			newItem.adjectives = "sharp";
			newItem.description = "Enhanced fingernails. The pointy-ends face out.";
			newItem.type = id;
			newItem.prop1 = 1;
			setB(newItem.flags, BIT_WEAPON | BIT_IMMOBILE);
			break;
		case ITEM_CLOTHES:
			newItem.name = "clothes";
			newItem.adjectives = "";
			newItem.description = "Although trendy, in your time of need, you feel these just aren't cutting it. \
With trolls and all that, you'd think there'd be some armor to pilfer.";
			setB(newItem.flags, BIT_ARMOR | BIT_IMMOBILE);
			newItem.prop1 = 0;
			break;
		case ITEM_RAGS:
			newItem.name = "rags";
			newItem.adjectives = "";
			newItem.description = "Although worn and moth-eaten, these thick tatters seems to provide a small amount \
of padding against conventional weaponry.";
			setB(newItem.flags, BIT_ARMOR | BIT_IMMOBILE);
			newItem.prop1 = 1;
			break;
		case ITEM_BOTTLE:
			newItem.name = "bottle";
			newItem.adjectives = "empty";
			newItem.description = "Although empty, it once held some great juice. You hold the bottle close to your \
heart and cherish the memory.";
			break;
		case ITEM_SMALL_POTION:
			newItem.name = "bottle";
			newItem.adjectives = "small red";
			newItem.description = "A clear bottle filled to the brim with a strange red liquid, sealed with a stopper. \
Your brain tells you to not drink strange liquids, but your heart is chanting 'chug it!'";
			setB(newItem.flags, BIT_POTION);
			newItem.prop1 = 1;
			break;
		case ITEM_LARGE_POTION:
			newItem.name = "bottle";
			newItem.adjectives = "large red";
			newItem.description = "A clear bottle filled to the brim with a strange red liquid, sealed with a stopper. \
This one seems unusually large.";
			setB(newItem.flags, BIT_POTION);
			newItem.prop1 = 3;
			break;
		case ITEM_ELIXIR:
			newItem.name = "bottle";
			newItem.adjectives = "tiny crystal";
			newItem.description = "A tiny crystal bottle filled with a strange purple liquid. You wonder why \
anyone would craft such a nice bottle for this - it's barely a swig...";
			setB(newItem.flags, BIT_POTION);
			newItem.prop1 = 5;
			break;
		case ITEM_RED_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large red steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE);
			newItem.prop1 = RED_1;
			break;
		case ITEM_GREEN_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large green steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE);
			newItem.prop1 = GREEN_1;
			break;
		case ITEM_BLUE_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large blue steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE);
			newItem.prop1 = BLUE_1;
			break;
		case ITEM_YELLOW_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large yellow steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE);
			newItem.prop1 = YELLOW_1;
			break;
		case ITEM_GOLD_DOOR:
			newItem.name = "door";
			newItem.adjectives = "large gold steel";
			newItem.description = "At around 12 feet high, this door is much larger than what you'd expect for a \
human. A fist-sized lock stands out at eye level, but otherwise, the fixture holds no other discernible qualities.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE | BIT_LOCKED);
			newItem.prop1 = BOSS_ENTRY;
			newItem.prop2 = ITEM_GOLD_KEY;
			newItem.sprop1 = "You approach the large golden door with your golden key.As you draw near, your key \
begins to glow, and one by one, each of the 4 colored locks fall to the ground. As the final lock hits the ground \
you hear a rumbling and the room starts to shake slightly. A blazing light shoots out from the door, and it slowly \
swings open.";
			break;
		case ITEM_SECRET_ENT:
			newItem.name = "door";
			newItem.adjectives = "large steel";
			newItem.description = "This massive steel slab is almost more wall than door; even if you had a means of \
opening it, it would require great strength to do so. An ornate indentation of a skull sits in the middle of the steel.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE | BIT_LOCKED);
			newItem.prop1 = SECRET_CHAMBER;
			newItem.prop2 = ITEM_GOLD_KEY;
			break;
		case ITEM_SECRET_EXIT:
			newItem.name = "door";
			newItem.adjectives = "large steel";
			newItem.description = "Huh - guess it was a door after all. No wonder - they kept the treasure room stocked! \
You almost wish you were a pirate, just so you could brag about all the booty you plundered";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE);
			newItem.prop1 = ENTRY;
			break;
		case ITEM_BURROW_ENT:
			newItem.name = "burrow";
			newItem.adjectives = "small rounded";
			newItem.description = "A small burrow is dug into the wall here, though by the markings, you aren't sure \
what dug it. If you were to squeeze, you could probably make your way through it.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE);
			newItem.prop1 = CORRIDOR;
			break;
		case ITEM_BURROW_EXIT:
			newItem.name = "hole";
			newItem.adjectives = "small rounded";
			newItem.description = "It's the hole you came in from. You think you could probably squeeze \
back through, but you'd probably get all dirty again.";
			setB(newItem.flags, BIT_EXIT | BIT_IMMOBILE);
			newItem.prop1 = ENTRY;
			break;
		case ITEM_RED_KEY:
			newItem.name = "key";
			newItem.adjectives = "large red";
			newItem.description = "Is this really supposed to be a key? It doesn't look like it'd fit in \
any tumbler you've ever seen. The size is understandable, but it doesn't seem to have any teeth to it. ";
			setB(newItem.flags, BIT_KEY | BIT_WEAPON);
			newItem.prop1 = 4;
			break;
		case ITEM_BLUE_KEY:
			newItem.name = "key";
			newItem.adjectives = "large blue";
			newItem.description = "You might as well call this thing a mace, because it sure doesn't look \
like a key. Maybe there's some special purpose to it, but you don't seem to be able to find it.";
			setB(newItem.flags, BIT_KEY | BIT_WEAPON);
			newItem.prop1 = 4;
			break;
		case ITEM_YELLOW_KEY:
			newItem.name = "key";
			newItem.adjectives = "large yellow";
			newItem.description = "Large and unembellished was the apparent motto of whoever designed this \
thing. If it were just a tad longer, you think it could make a good prybar.";
			setB(newItem.flags, BIT_KEY | BIT_WEAPON);
			newItem.prop1 = 4;
			break;
		case ITEM_GREEN_KEY:
			newItem.name = "key";
			newItem.adjectives = "large green";
			newItem.description = "This thing really looks like it was designed for trolls - a long, crude \
blunt instrument with a pointy end. It could be some some radical new door-opening technology the world has \
never seen before, or maybe it's just an object to cause blunt-force trauma? Hard to say.";
			setB(newItem.flags, BIT_KEY | BIT_WEAPON);
			newItem.prop1 = 4;
			break;
		case ITEM_GOLD_KEY:
			newItem.name = "key";
			newItem.adjectives = "solid gold";
			newItem.description = "Wow. Guess those were keys after all. Maybe you underestimated those trolls? \
Despite this thing being made of SOLID GOLD, it's surprisingly light. You could probably even show off with this \
thing, if you ever make it back to the village";
			setB(newItem.flags, BIT_KEY | BIT_WEAPON);
			newItem.prop1 = 10;

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
		case ITEM_2H_SWORD:
			newItem.name = "sword";
			newItem.adjectives = "two-handed";
			newItem.description = "A massive two-handed sword. Anyone hit by this thing is going to be \
in some serious pain";
			setB(newItem.flags, BIT_WEAPON);
			newItem.prop1 = 9;
		break;
		case ITEM_CLOTH_ROBE:
			newItem.name = "robe";
			newItem.adjectives = "padded linen";
			newItem.description = "Though a bit regal-looking, this robe provides basic protection against \
most weapons. It's not gonna stop an arrow - but you get what you pay for.";
			setB(newItem.flags, BIT_ARMOR);
			newItem.prop1 = 1;
			break;
		case ITEM_ROTTING_LEATHER:
			newItem.name = "armor";
			newItem.adjectives = "rotting leather";
			newItem.description = "You can't tell if this started out as armor, or if it became armor \
after receiving a nice coat of mold from mother nature. Hopefully it holds together";
			setB(newItem.flags, BIT_ARMOR);
			newItem.prop1 = 2;
			break;
		case ITEM_STUDDED_LEATHER:
			newItem.name = "armor";
			newItem.adjectives = "studded leather";
			newItem.description = "This stuff looks pretty decrepit, but the studs are still intact \
and the armor smells like it's only mildly been used. Beats nothing.";
			setB(newItem.flags, BIT_ARMOR);
			newItem.prop1 = 3;
			break;
		case ITEM_CHAIN_SHIRT:
			newItem.name = "shirt";
			newItem.adjectives = "old chain";
			newItem.description = "This shirt is pretty light, all things considered - it doesn't restrict \
your mobility, yet also provides decent protection from most attacks. Not too shabby.";
			setB(newItem.flags, BIT_ARMOR);
			newItem.prop1 = 4;
			break;
		case ITEM_RUSTY_PLATE:
			newItem.name = "armor";
			newItem.adjectives = "rusty plate";
			newItem.description = "What is garbage like this doing in a place like this? Though surprising \
durable, time has not been nice to this rust-bucket - you feel like you might contract lockjaw just from \
looking at it.";
			setB(newItem.flags, BIT_ARMOR);
			newItem.prop1 = 5;
			break;
		case ITEM_SHINY_PLATE:
			newItem.name = "armor";
			newItem.adjectives = "shiny plate";
			newItem.description = "What is fantastic armor like this doing in a place like this? You feel \
more powerful just looking at it - what kind of monster was able to take someone down in a suit of this? \
Wearing a trash-can has never been so amazing.";
			setB(newItem.flags, BIT_ARMOR);
			newItem.prop1 = 10;
			break;
		default:
			cout << "createItem(): unknown id (" << id << ")" << endl;
		break;
	}
	newItem.type = id;
	return newItem;
};

void initRooms() {
	rooms = new game_room[GAME_ROOM_SIZE];

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
	setB(r->flags, ROOM_NEUTRAL | ROOM_RUNNING);

	r = &rooms[FOREST_2];
	r->transition = "As you bustle through the dark forest, it dawns on you that you no longer \
have any idea where you are - you've never been this deep before, let alone in complete darkness. That doesn't \
stop your running though - trolls are much scarier than darkness. Suddenly, with one errant step, the ground \
exits beneath you, and you find yourself tumbling into the unknown.";
	setB(r->flags, ROOM_NEUTRAL | ROOM_RUNNING);

	r = &rooms[FOREST_3];
	r->transition = "You continue your nascent plunge, devoid of rhyme or reason. After jarring \
yourself several times against the mysteries of the night, you finally hit ground with a dull thud, knocking \
the breath from your chest. You take a second to recover, only to realize that you there is no way out of this \
pit as it dawns on you may have fallen into a trap. You feel around until you locate a gap in the darkness and \
frantically begin your descent, hopefully out of danger \
for a change?";
	setB(r->flags, ROOM_NEUTRAL | ROOM_RUNNING);

	r = &rooms[PIT_1];
	r->transition = "As you continue deeper down into the hole, you catch a glimpse of light in the distance, \
but give pause when you realize that there is no natural light underground. Maybe someone else made it \
out too? You slow your stride and begin to ready yourself for action.";
	setB(r->flags, ROOM_NEUTRAL | ROOM_RUNNING);

	r = &rooms[PIT_2];
	r->description = "A large chasm has taken shape between you and the lights, bringing your journey to a halt. \
It seems your only hope might be to to jump it.";
	r->transition = "Gathering up your courage, you take a flying leap over the chasm! As you pass up through the \
air down to solid ground below you, the fear that was gripped you earlier has been replaced by exhilaration. Maybe \
this is what you were born to do?";
	setB(r->flags, ROOM_NOEXIT | ROOM_NEUTRAL | ROOM_JUMPEXIT);
	r->prop1 = ENTRY;

	r = &rooms[ENTRY];
	r->description = "You are now in front of a large metal door. There's a hole you can crawl \
through";
	r->transition = "You crawl down into the hole and begin shimmying through. Loose dirt flies up in \
your hair and on your face and clothes(not that you weren't already filthy). With significant labor, \
you squeeze through";
	setB(r->flags, ROOM_NOEXIT);
	r->items.push_back(createItem(ITEM_SECRET_ENT));
	r->items.push_back(createItem(ITEM_BURROW_ENT));
	r->items.push_back(createItem(ITEM_GOLD_KEY));
	r->items.push_back(createItem(ITEM_RANDOM_TREASURE_BOX));

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
	setB(r->flags, ROOM_NEUTRAL | ROOM_NOEXIT);
	r->items.push_back(createItem(ITEM_BLUE_DOOR));
	r->items.push_back(createItem(ITEM_GREEN_DOOR));
	r->items.push_back(createItem(ITEM_RED_DOOR));
	r->items.push_back(createItem(ITEM_YELLOW_DOOR));
	r->items.push_back(createItem(ITEM_GOLD_DOOR));
	r->items.push_back(createItem(ITEM_BURROW_EXIT));
	r->items.push_back(createItem(ITEM_RANDOM_TREASURE_BOX));

	//If this description needs to be less detailed in order to reuse the room later in the program, we can go with something like, You enter
	//the main room of the cave system. Opposite you are 5 doors, one red, one green, one blue and one yellow. In the center is a massive
	//golden door. Which door will you go through?

	r = &rooms[RED_EXIT];
	r->name = "Red Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	setB(r->flags, ROOM_NEUTRAL);
	// transition text: ejected back into corridor

	r = &rooms[GREEN_EXIT];
	r->name = "Green Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	setB(r->flags, ROOM_NEUTRAL);

	r = &rooms[BLUE_EXIT];
	r->name = "Blue Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	setB(r->flags, ROOM_NEUTRAL);

	r = &rooms[YELLOW_EXIT];
	r->name = "Yellow Door, Narrow Passage";
	r->description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	setB(r->flags, ROOM_NEUTRAL);

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
	r->description = "";

	r = &rooms[BOSS];
	r->name = "Large Throne Room";
	r->description = "As you step through the doors, you find yourself in a brightly lit throne room, with tall vaulted ceilings and many paintings on the \
	\nthe wall. There is a roaring fire in the fireplace on one wall, and across from you you see a massive golden throne.";
	r->transition= "As you exit the caves, the sunlight hits your face causing you to squint in the brightness momentarily. You realize that you've been in \
the caves all night. You breathe a sigh of relief as you realize that it's over and you wander off into the forest in search of a place to sleep.";
	setB(r->flags, ROOM_NEUTRAL);
	r->monster = createMonster(MONSTER_TROLL_KING);

	r = &rooms[EXIT];
	r->name = "Dungeon Exit";
	r->description = "As you exit the caves, the sunlight hits your face causing you to squint in the brightness momentarily. You realize that you've been in \
the caves all night. You breathe a sigh of relief as you realize that it's over and you wander off into the forest in search of a place to sleep.";
	setB(r->flags, ROOM_NEUTRAL | ROOM_WIN | ROOM_NOEXIT);

	r = &rooms[SECRET_CHAMBER];
	r->name = "Hidden Chamber";
	r->description = "As you exit the caves, the sunlight hits your face causing you to squint in the brightness momentarily. You realize that you've been in\
	\n the caves all night. You breathe a sigh of relief as you realize that it's over and you wander off into the forest in search of ";
	setB(r->flags, ROOM_NEUTRAL | ROOM_NOEXIT);
	r->items.push_back(createItem(ITEM_SECRET_EXIT));
	// populated rooms - TODO

	for (int i = 0; i < GAME_ROOM_SIZE; i++) {
		game_room *r = &rooms[i];
		game_monster *m = &r->monster;
		game_trap *t = &rooms[i].trap;
		if (!checkB(r->flags, ROOM_NEUTRAL)) {
			t->isArmed = rand() % 2;
			r->hasRelic = rand() % 2;
			if (rand() % 4 != 0) {
				*m = createMonster(MONSTER_RANDOM);
			}
		}
	}
};