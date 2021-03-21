#pragma once

#include <list>

#include "player.h"
#include "MechanicEntry.h"

struct PlayerEntry
{
	Player* player = nullptr;
	std::list<MechanicEntry> entries;

	unsigned int pulls = 0;
	unsigned int mechanics_neutral = 0;
	unsigned int mechanics_failed = 0;

	unsigned int downs = 0;              //number of times the player has downed
	unsigned int deaths = 0;              //number of times the player has completely died

	uint64_t last_hit_time = 0;       //time player was last hit with a mechanic

	PlayerEntry(Player* new_player);

	bool operator==(uintptr_t other_id) { return player && *player == other_id; };
	bool operator==(std::string other_str) { return player && *player == other_str; };
	void addMechanicEntry(uint64_t new_time, Mechanic* new_mechanic, const Boss* new_boss);
	void addPull(const Boss* new_boss);
	int getMechanicsTotal();
	uint64_t getLastMechanicHitTime(uint32_t new_mechanic);

	void down();
	void dead();
	void rally();
	void fixDoubleDown(); //manual case to fix vapor form counting as 2 downs

	bool isRelevant();     //if player is relevant for displaying

	std::string toString();

	void combatEnter();
	void combatExit();
	void setStabTime(uint64_t new_stab_time);
};