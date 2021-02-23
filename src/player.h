#pragma once

#include <stdint.h>
#include <string>
#include <cstring>
#include <vector>
#include <mutex>
#include "arcdps_datastructures.h"
#include "bosses.h"

struct Player
{
    std::string name = "";
    std::string account = "";
	std::string name_account_combo = "";//name and account in same string, used for chart display
    uintptr_t id = 0;            //instance id
	bool is_self = false;		//is the local player
	bool is_downed = false;     //is currently is down state
    bool in_squad = true;          //currently in squad
	bool in_combat = false;
	uint64_t last_stab_time = 0;  //time stability is going to expire

    Player(ag* new_player);
    Player(char* new_name, char* new_account, uintptr_t new_id, bool new_is_self);

	bool operator==(Player* other_player);
	bool operator==(uintptr_t other_id);
	bool operator==(std::string other_str);
};
