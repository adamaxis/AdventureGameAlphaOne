#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

// file reading propotypes and constants
extern const char *FILE_HISCORES;
extern const int MAX_SCORES;
extern int hiScore[];
void newHiScores();
void readHiScores();



// bit manipulation inline functions: setbit, clearbit, togglebit, checkbit
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

// item_bit_states - bit-states for item flags.
// 1 = 0000 0001, 2 = 0000 0010, 4 = 0000 0100, etc.. could have been done as hex, IE 0x01, 0x02, but decided this was more readable
enum item_bit_states {
	BIT_IMMOBILE = 1,
	BIT_EXIT = 2,
	BIT_BLOCKED_VISIBLE = 4,
	BIT_LOCKED = 8,
	BIT_KEY = 16,
	BIT_CONTAINER = 32,
	BIT_POTION = 64,
	BIT_WEAPON = 128,
	BIT_ARMOR = 256,
	BIT_GOLD = 512
};

// room_bit_states - bit-states for room flags. also used once on player flags(late design change)
enum room_bit_states {
	ROOM_DARK = 1,
	ROOM_RUNNING = 2,
	ROOM_NEUTRAL = 4,
	ROOM_NOEXIT = 8,
	ROOM_JUMPEXIT = 16,
	ROOM_WIN = 32
};

// game_states - possible states for game menu\status
enum game_states {
	STATE_EXIT = 0,
	STATE_MENU,
	STATE_INIT,
	STATE_PLAYING,
	STATE_GAMEOVER
};

// menu_states - possible states for main menu choice
enum menu_states {
	MENU_HISCORES = 1,
	MENU_PLAY,
	MENU_EXIT
};

// game_commands - possible states for command entered
enum game_commands {
	COMMAND_UNKNOWN = 0,
	COMMAND_CONTINUE,
	COMMAND_FIGHT,
	COMMAND_SEARCH,
	COMMAND_GO,
	COMMAND_STATUS,
	COMMAND_HEALTH,
	COMMAND_WEALTH,
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
	COMMAND_WIELD,
	COMMAND_VERBOSITY,
	COMMAND_APPRAISE
};

// states for different types of objects - used in conjunction with game_command for target identification
enum game_object_types {
	TYPE_NONE,
	TYPE_ITEM,
	TYPE_MONSTER,
	TYPE_PLAYER,
	TYPE_ROOM,
	TYPE_UNKNOWN
};

// game_monsters - states for different types of monsters, organized by difficult(-5 to 5) -10 for not_a_monster and 10 for final boss
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
	MONSTER_TROLL_GUARD,
	MONSTER_TROLL_KING = 10,
	MONSTER_RANDOM				// special case
};

// game_locations - tags for different rooms, each corresponding to a room within the room array
enum game_locations {
	FOREST, FOREST_2, FOREST_3,
	PIT_1, PIT_2,
	ENTRY,
	CORRIDOR,
	RED_1, RED_2, RED_3, RED_4, RED_5, RED_EXIT,
	GREEN_1, GREEN_2, GREEN_3, GREEN_4, GREEN_5, GREEN_EXIT,
	BLUE_1, BLUE_2, BLUE_3, BLUE_4, BLUE_5, BLUE_EXIT,
	YELLOW_1, YELLOW_2, YELLOW_3, YELLOW_4, YELLOW_5, YELLOW_EXIT,
	BOSS_ENTRY, BOSS,
	EXIT,
	SECRET_CHAMBER,
	END							// marker for room array size
};

// constant for rooms array
const int GAME_ROOM_SIZE = END + 1;


// game_traps - gives each trap a unique ID
enum game_traps {				// right now there is only one type of trap - didn't get time to flesh them out
	TRAP_NONE = 0,
	TRAP_NORMAL
};

// game_items enum - gives each item a unique ID
enum game_items {				// all negative entries are natural weapons/armor
	ITEM_FISTS = -10,
	ITEM_CLAWS,
	ITEM_CLOTHES,
	ITEM_RAGS,
	ITEM_NONE = 0,
	ITEM_RED_KEY, ITEM_BLUE_KEY, ITEM_YELLOW_KEY, ITEM_GREEN_KEY, ITEM_GOLD_KEY,			// colored keys
	ITEM_RED_DOOR, ITEM_BLUE_DOOR, ITEM_YELLOW_DOOR, ITEM_GREEN_DOOR, ITEM_GOLD_DOOR,		// corresponding colored doors
	ITEM_BURROW_ENT, ITEM_BURROW_EXIT,			// burrow entrance and exit
	ITEM_SECRET_ENT, ITEM_SECRET_EXIT,			// secret chamber entrance and exit
	ITEM_WEAPON = 100,			// weapon drops begin
	ITEM_DAGGER,
	ITEM_SCIMITAR,
	ITEM_RAPIER,
	ITEM_LONGSWORD,
	ITEM_BROADSWORD,
	ITEM_2H_SWORD,
	ITEM_WEAPON_END,			// weapon drops end
	ITEM_ARMOR = 200,				// armor drops begin
	ITEM_CLOTH_ROBE,
	ITEM_ROTTING_LEATHER,
	ITEM_STUDDED_LEATHER,
	ITEM_CHAIN_SHIRT,
	ITEM_RUSTY_PLATE,
	ITEM_ARMOR_END,				// armor drops end
	ITEM_ITEM = 300,			// item drops begin
	ITEM_GOLD_PIECES, ITEM_GOLD_HANDFUL, ITEM_GOLD_PILE,
	ITEM_RANDOM_TREASURE_BOX,
	ITEM_SMALL_POTION,
	ITEM_LARGE_POTION,
	ITEM_ELIXIR,
	ITEM_ITEM_END,				// item drops end
	ITEM_BOTTLE,				// special case items go here
	ITEM_SHINY_PLATE,
	ITEM_RED_CHEST,
	ITEM_GREEN_CHEST,
	ITEM_BLUE_CHEST,
	ITEM_YELLOW_CHEST,
	ITEM_BIG_TREASURE_CHEST
};



// game_item structure - for items in the game
struct game_object {
	string s1, s2, s3, s4, s5;
	int i1, i2, i3, i4, i5;
	long l1, l2, l3, l4, l5;
	string description;
	int type;
	int kills;
	long flags, prop1, prop2, prop3;
	string sprop1;
	int id;

	// flags reference
	// isExit - prop1 contains exit ID, sporp1 contains exit text
	// isLocked - prop2 contains key item ID
	// isArmor - prop1 contains armor def
	// isWeapon - prop1 contains weapon dmg
	// isContainer - prop1 contains containers[] array index
	// isGold - prop3 contains value

	// constructor
	game_object() {
		// give each item a unique id
		id = rand();

		// default values
		clear();
	}

	// clear() - clears all object data
	void clear() {
		s1 = s2 = s3 = s4 = s5 = "";
		type = ITEM_NONE;
		flags = prop1 = prop2 = prop3 = 0;
	}

	// copy(item) - copies the provided item data to this object(including id)
	void copy(game_object c) {
		memcpy(this, &c, sizeof(c));
	}

};


// game_item structure - for items in the game
struct game_item {
	string name;
	string adjectives;
	string description;
	int type;
	int kills;
	long flags, prop1, prop2, prop3;
	string sprop1;
	int id;

	// flags reference
	// isExit - prop1 contains exit ID, sporp1 contains exit text
	// isLocked - prop2 contains key item ID
	// isArmor - prop1 contains armor def
	// isWeapon - prop1 contains weapon dmg
	// isContainer - prop1 contains containers[] array index
	// isGold - prop3 contains value

	// constructor
	game_item() {
		// give each item a unique id
		id = rand();

		// default values
		clear();
	}

	// clear() - clears all object data
	void clear() {
		name = adjectives = description = sprop1 = "";
		type = ITEM_NONE;
		flags = prop1 = prop2 = prop3 = 0;
	}

	// copy(item) - copies the provided item data to this object(including id)
	void copy(game_item c) {
		name = c.name;
		adjectives = c.adjectives;
		description = c.description;
		type = c.type;
		flags = c.flags;
		prop1 = c.prop1;
		prop2 = c.prop2;
		prop3 = c.prop3;
		sprop1 = c.sprop1;
		id = c.id;
	}

};

// dependant-function prototypes
game_item createItem(int);

// item value constants
const int VALUE_MINOR = 1;
const int VALUE_MAJOR = 10;
const int VALUE_AWESOME = 100;


// target resolution prototypes
void resolveTarget(vector<string> &);

// room data function prototype
void initRooms();

// d20 function prototype
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
void doStatus(vector<string> &);
void doWealth(vector<string> &);
void doHealth(vector<string> &);
void doInventory(vector<string> &);
void doTake(vector<string> &);
void doWield(vector<string> &);
void doWear(vector<string> &);
void doVerbosity(vector<string> &);
void doAppraise(vector<string> &);

// post-action function prototype
void doPostCheck(vector<string> &);

/* A lot of these functions are templates because
  it's easier to manage them that way. */

  // trap generics
  // isArmed(T) - self-explanatory
template<class T>
bool isArmed(T *t) { return t->isArmed; };

template<class T>
bool isTrapped(T *t) { return t->trap.isArmed; };

// weapon functions
// getWeaponName(T) - returns weapon name of player or monster
template<class T>
string getWeaponName(T *entity) { return entity->weapon.name; };

// getDamage(T) - returns weapon damage of player or monster
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
int getValue(T & entity) { return (entity.prop3); };

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

template<class T>
bool isGold(T *i) { return checkB(i->flags, BIT_GOLD); };


// room bit checkers
template<class T>
bool hasExit(T *i) { return !checkB(i->flags, ROOM_NOEXIT); };

template<class T>
bool isNeutral(T *r) { return checkB(r->flags, ROOM_NEUTRAL); };

template<class T>
bool isRunning(T *r) { return checkB(r->flags, ROOM_RUNNING); };

// maximum player HP constant
const int MAX_HP = 5;

// player struct - self explanatory
struct game_player {
public:
	string name;
	int kills, trapWinCount, trapFailCount, gold, HP, rloc;
	long flags;
	vector<game_item> items;
	game_item weapon, armor;

	// constructor
	game_player() {
		gold = kills = trapWinCount = trapFailCount = flags = 0;
		rloc = FOREST;
		weapon = createItem(ITEM_FISTS);
		armor = createItem(ITEM_CLOTHES);
		HP = MAX_HP;
	}

	int getKills() { return kills; };
	int getTrapTotalCount() { return (trapWinCount + trapFailCount); };
	int getTrapWinCount() { return trapWinCount; };
	int getTrapFailCount() { return trapFailCount; };
	int getGold() { return gold; };
};

// monster struct, for fleshing out monsters
struct game_monster {
	string name, description, adjectives;
	int HP;
	game_item weapon, armor;
	vector<game_item> items;
	int difficulty;

	// constructor
	game_monster() {
		difficulty = MONSTER_NONE;
		description = "none";
		HP = 0;
		weapon = createItem(ITEM_CLAWS);
		armor = createItem(ITEM_RAGS);
	}
};

// monster creation and death prototypes
game_monster createMonster(int = MONSTER_NONE);
void monsterDeath(game_monster *);

/* trap struct
 traps were supposed to be independent objects, but due to time constraints,
 they're just an accessory to game_room */
struct game_trap {
	string name;
	bool isArmed;
	int difficulty;

	game_trap() {
		name = "trap";
		isArmed = false;
		difficulty = TRAP_NONE;
	}
};

// room struct, for fleshing out rooms and their attributes
struct game_room {
	bool hasVisited;
	string name, description, transition;
	game_monster monster;
	vector<game_item> items;
	long flags;
	long prop1, prop2;
	game_trap trap;

	// constructor
	game_room() {
		flags = prop1 = prop2 = 0;
		name = description = transition = "";
		hasVisited = false;
	}
};

// command structure - for handling player input and holding targetting information
struct game_command {
	int cmd = COMMAND_UNKNOWN;
	int type = 0, ptype = 0;
	void *target, *parent;

	// constructor
	void clear() {
		cmd = COMMAND_UNKNOWN;
		type = ptype = TYPE_NONE;
		target = nullptr, parent = nullptr;
	}
};

// command parser/text manipulation function prototypes
vector<string> delimit(const string &, char);
string trimFirst(string &, char);
bool isMatch(string, string);
int determineCommand(vector<string> &);
vector<string> getInput();
game_command searchByType(long = 0);
game_command searchByString(vector<string> &);


#endif