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
	COMMAND_ESCAPE,
	COMMAND_ENTER
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
string trimFirst(string &, char);
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
	int difficulty;

	game_monster() {
		difficulty = MONSTER_NONE;
		HP = 0;
		weapon = createItem(ITEM_FIST);
	}
};

game_monster createMonster(int = MONSTER_NONE);

struct game_trap {
	string name, description, adjectives;
	bool isArmed = false;
	int difficulty = TRAP_NONE;
};

// room struct, for fleshing out rooms and their attributes
struct game_room {
	bool hasRelic, beenVisited;
	string name, description, describer;
	game_monster monster;
	vector<game_item> items;
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
string getDescriber(T & entity) {
	return (entity->adjectives + " " + entity->name);
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

						case COMMAND_ENTER:
							
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

int determineAction(vector<string> input) {
	game_player *p = player;
	game_room *r = &rooms[p->rloc];
	game_monster *m = &r->monster;
	vector<game_item> *ir = &r->items;
	vector<game_item> *ip = &p->items;
	string cmd, target;
	int cid;
	// identify command
	cmd = input.at(0);
	if (isMatch("fight", cmd)) {
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
		if(input.size() > 1) {
			target = input.at(input.size() -1);
			for(int i=0; i < ir->size(); i++) {
				if(isMatch(ir->at(i).name, target)) cout << "Affirmitive match: " << ir->at(i).name << endl;
			}
			for (int i = 0; i < ip->size(); i++) {
				if (isMatch(ip->at(i).name, target)) cout << "Affirmitive match: " << ip->at(i).name << endl;
			}
			if (isMatch(p->weapon.name, target)) cout << "Affirmitive match: " << p->weapon.name << endl;
			if (isMatch(m->name, target)) cout << "Affirmitive match: " << m->name << endl;
			//if (isMatch(, target)) cout << "Affirmitive match: " << m->name << endl;
			// search player if (isMatch(p->name, target)) cout << "Affirmitive match: " << m->name << endl;
		}
		cid = COMMAND_LOOK;
	} else if(cmd.compare("") == 0) {
		cid = COMMAND_ENTER;
	} else {
		cout << "Unknown command";
		cid = COMMAND_UNKNOWN;
	}
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
		int pHit = d20(player->bonus);
		int mHit = d20(m->difficulty);
		cout << "You rolled: " << pHit << endl;
		cout << m->name << " rolled: " << mHit << endl;
		if(pHit >= mHit) {
			cout << "You out-fox the " + m->name + " and land a solid blow with your " << getWeapon(p) << "!" << endl;
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

// doContinue - CONTINUE command, which moves the player forward
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

// doLook - LOOK command, which shows the player what's happening in the room
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
			cout << "A " << getDescriber(m) << " stands before you." << endl;
		} else if(!isLiving(m) && m->difficulty != MONSTER_NONE) {
			cout << "The corpse of a " << m->name << " lies motionless here." << endl;
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
		cout << "RUN not implemented here yet - defaulting to CONTINUE" << endl;
		doContinue();
	}
}


vector<string> getInput() {
	string input;
	getline(cin, input);
	cout << input << endl;
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
	rooms[FOREST_2].description = "You continue running, trying to outpace the trolls in pursuit. The branches are like outstretched\
	\nfingers clawing at your face and clothes. One thought dominates all, 'Keep moving, just keep moving.'";
	rooms[FOREST_3].description = "As you make your way through the dark forest, you realize that you no longer have any idea where you\
	\nare. You've never been this deep into the forest before, let alone at night. You keep running, putting one foot in front of the\
	\nother. Suddenly there is no more ground beneath your feet. You tumble down into the darkness.";
	rooms[ENTRY].name = "Main Room, Several Doors";
	rooms[ENTRY].description = "You hit the ground with a dull thud, and the breath is knoked from your chest.You take a few moments\
	\n and recover, eventually getting up and dusting yourself off. you like around and realize you've fallen to the bottom of a rather\
	\nlarge pit. There is no way back up to the forest above. You scan the pit and see an opening on the wall. You approach it. As you \
	\n approach the opening you notice that torches line the walls lighting the room you've just walked into. It's dim, but you can see\
	\nthat there are five doors on the wall opposite you. As you cautiously walk closer to the doors, you hear a loud rumble, as if \
	\nthe earth itself is being split in two. Without warning the entrance behind you caves in. You're trapped. Your only choice is to\
	\ngo through one of the doors ahead of you. As you move closer, you can make out more details about the doors. On the left there are\
	\ntwo doors, one red and one green. On the right there are two more, one blue and one yellow. In the center there is a massive, ornately\
	\ndecorated door that appears to be made of solid gold. Where would you like to go first?";
	//If this description needs to be less detailed in order to reuse the room later in the program, we can go with something like, You enter
	//the main room of the cave system. Opposite you are 5 doors, one red, one green, one blue and one yellow. In the center is a massive
	//golden door. Which door will you go through?
	rooms[HALLWAY].name = "Corridor, Chamber";
	rooms[HALLWAY].description = "You open the door and head down the dimly lit passage. After a short while, the passage opens into a room.";
	rooms[RED_EXIT].name = "Red Door, Narrow Passage";
	rooms[RED_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[GREEN_EXIT].name = "Green Door, Narrow Passage";
	rooms[GREEN_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[BLUE_EXIT].name = "Blue Door, Narrow Passage";
	rooms[BLUE_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";
	rooms[YELLOW_EXIT].name = "Yellow Door, Narrow Passage";
	rooms[YELLOW_EXIT].description = "You're meandering through what seems an endless passage. Light is dim, and the only direction is forward.";

	// populated rooms - TODO
	rooms[RED_1].name = "Small Room";
	rooms[RED_1].description = "You come into a room that is fairly small and nondescript. It is lit by a few torches on the wall. What would\
	you like to do?";
	rooms[RED_2].name = "Small Pantry";
	rooms[RED_2].description = "You enter the room, and you can tell that it is, or was, a pantry. There are cupboards along the walls and what\
	appears to be several preparation tables in the center of the room. What would you like to do?";
	rooms[RED_3].name = "Large Sleeping Quarters";
	rooms[RED_3].description = "When you enter this room you see several rows of bunk beds that stretch off into the distance, fading into the \
	dark. As far as you can tell, there is nothing, or no one, sleeping in the beds. What will you do?";
	rooms[RED_4].name = "Medium Library";
	rooms[RED_4].description = "As you cross the threshhold of this room, you see rows of bookcases all filled to the brim with books. This is \
	one of the brightest rooms you've been in so far, thanks to the chandeliers hanging from the ceiling. What would you like to do?";
	rooms[RED_5].name = "Large Forge";
	rooms[RED_5].description = "As you enter the room, the first thing you notice is that it is very hot. You see the forge and realize that must\
	be where the heat is coming from. There are workbenches all over the room with tools scattered haphazardly across them. What would you like to\
	do?";
	rooms[GREEN_1].name = "Small Storage Room";
	rooms[GREEN_1].description = "This room is another small storage room. It has several cabinets in it, as well as a stack of crates in the corner\
	What will you do?";
	rooms[GREEN_2].name = "Medium Kitchen";
	rooms[GREEN_2].description = "This room has several prep tables as well as a sink to wash dishes, and several ovens. You think of the last meal \
	that you had. Beef stew. Your stomach rumbles and you're reminded of your hunger. What do you do?";
	rooms[GREEN_3].name = "Medium Distillery";
	rooms[GREEN_3].description = "As you enter the room, you see the remnants of several stills along the edges of the room. In the center is a table\
	with several empty bottles scattered across the top. What would you like to do?";
	rooms[GREEN_4].name = "Large Training Room";
	rooms[GREEN_4].description = "Inside this room you see racks of practice weapons as well as several training dummies, two archery targets and a \
	first aid kit. What are you going to do?";
	rooms[GREEN_5].name = "Large Mage Workshop";
	rooms[GREEN_5].description = "As you enter this room you see several arcane tables set up. One of the tables has several crystals on it. One of \
	the crystals is glowing faintly, but you decide it's best if you leave it alone. What will you do?";
	
	rooms[BLUE_1].name = "Medium Indoor Garden";
	rooms[BLUE_1].description = "As you come into this room, the first thing that hits you is the smell. There is the overwhelming smell of manure. As\
	you cover your nose and mouth to keep from gagging, you see that all manner of moss, lichens, and mushrooms are growing in planter boxes filled \
	with what you can only guess is manure. What are going to do?";
	rooms[BLUE_2].name = "Large Library";
	rooms[BLUE_2].description = "In this room there are hundreds of bookcases teeming with books. As you glance around you see several that appear to be\
	bound in solid gold. They are beautiful but you decide to keep moving. What will you do?";
	rooms[BLUE_3].name = "Burnt Mage Workshop";
	rooms[BLUE_3].description = "Once in this room you can see that it is a mage's workshop. Or it was. It seems as though something went horribly wrong\
	when it was last used. Everything inside is burnt to a crisp, and in the center of the room are the splintered remains of an arcane table. What would\
	you like to do?";
	rooms[BLUE_4].name = "Large Pantry";
	rooms[BLUE_4].description = "You emerge into a large pantry with al sorts of dried meats and cheeses hanging from pegs in the rafters. Despite your \
	hunger, you know that you should not stop and eat. What will you do?";
	rooms[BLUE_5].name = "Large Archery Range";
	rooms[BLUE_5].description = "Upon entering this room, you see that it is a large indoor archery range. There is a row of archery targets that stretches\
	off into the depths of the darkness. What are you going to do?";
	rooms[YELLOW_1].name = "Large Tavern";
	rooms[YELLOW_1].description = "You come into this room and see that it is a tavern. There are several tables set up, with chairs, as well as a row of bar\
	stools set up along the bar. You see the tap handles behind the bar and you wish that you had the time to enjoy a pint. What would you like to do?";
	rooms[YELLOW_2].name = "Large Armory";
	rooms[YELLOW_2].description = "Once you are in this room, you see racks of weapons lined up in the room. All manner of weapons are stored in the racks,\
	swords, maces, warhammers, spears, and many more. In the center of the room is a row of many workbenches littered with an assortment of hammers, tongs\
	whetstones and other various tools. What will you do?";
	rooms[YELLOW_3].name = "Large Dining Hall";
		rooms[YELLOW_3].description = "In this room you see rows of tables lined up stretching into the distance. There are plates at the tables and a candleabra\
	on each table. What are you going to do?";
	rooms[YELLOW_4].name = "Medium Latrine";
	rooms[YELLOW_4].description = "You enter this room and see that there are several stalls with toilets on one side, and on the other there are tubs lined up\
	for bathing. You are surprised that it doesn't smell worse than it does. What would you like to do?";
	rooms[YELLOW_5].name = "Large Stable"; \
		rooms[YELLOW_5].description = "Upon entering this room, you realize you've stumbled across a stable. There are several horses in the stalls one side of the\
	room and on the other there are several large bales of hay. You are thankful for the signs of life the horses bring you. What will you do?";
	rooms[BOSS_ENTRY].name = "Large Golden Door";
	rooms[BOSS_ENTRY].description = "You approach the large golden door with the four keys. As you unlock each lock, one by one, they fall to the ground. As the\
	final lock hits the ground you hear a rumbling and the room starts to shake slightly. The massive doors swing open and there is a blazinf light on the other\
	side of the doors. What are you going to do?";
	rooms[BOSS].name = "Large Throne Room";
	rooms[BOSS].description = "As you step through the doors, you find yourself in a brightly lit throne room, with tall vaulted ceilings and many paintings on the \
	\nthe wall. There is a roaring fire in the fireplace on one wall, and across from you you see a massive golden throne. What would you like to do?";
	rooms[EXIT].name = "Dungeon Exit";
	rooms[EXIT].description = "As you exit the caves, the sunlight hits your face causing you to squint in the brightness momentarily. You realize that you've been in\
	\n the caves all night. You breathe a sigh of relief as you realize that it's over and you wander off into the forest in search of ";
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