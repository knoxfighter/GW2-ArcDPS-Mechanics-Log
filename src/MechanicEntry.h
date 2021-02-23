#pragma once

#include <inttypes.h>
#include <string>

#include "mechanics.h"
#include "player.h"
#include "bosses.h"

class MechanicEntry
{
public:
	Mechanic* mechanic = nullptr;
	Player* player = nullptr;
	uint16_t hits = 0;
	Boss* current_boss = nullptr;
	uint16_t pulls = 0;
	uint64_t last_hit_time = 0;

	MechanicEntry(uint64_t new_time, Mechanic* new_mechanic, Player* new_player, Boss* new_boss);


	bool operator==(uint32_t other_id) { return mechanic && mechanic->ids[0] == other_id; };
	std::string toString();
	void addPull(const Boss* new_boss);
	bool isRelevant();
};
