#pragma once

#include <string>
#include <vector>
#include <utility>

#include "arcdps_datastructures.h"
#include "player.h"
#include "skill_ids.h"
#include "npc_ids.h"
#include "helpers.h"

extern bool has_logged_mechanic;

const unsigned int max_ids_per_mechanic = 10;

enum Verbosity
{
	verbosity_chart = 1 << 0,
	verbosity_log = 1 << 1,
	verbosity_all = (verbosity_chart | verbosity_log),
};

enum TargetLocation
{
	target_location_src,
	target_location_dst,
};

struct Mechanic
{
	std::string name = ""; //name of mechanic
	std::string name_internal = ""; //name of skill in skilldef
	std::string name_chart = ""; //name in chart (boss name - mechanic name)
	std::string name_ini = ""; //name used for ini saving
	std::string description = ""; //detailed description of what the mechanic is
	uint32_t ids[max_ids_per_mechanic] = { 0 }; //skill ids;
	size_t ids_size = 0;
	Boss* boss = &boss_generic;//required boss, ignored if null
	uint64_t frequency_player = 2000; //minimum time between instances of this mechanic per player(ms)
	uint64_t frequency_global = 0; //minimum time between instances of this mechanic globally(ms)
	uint64_t last_hit_time = 0; //time of last instance of mechanic
	uint8_t is_activation = ACTV_NONE;//required is_activation type from cbtevent
	uint8_t is_buffremove = CBTB_NONE;//required is_buffremove type from cbtevent
	int32_t overstack_value = -1;//required overstack value, -1 means accept any value
	int32_t value = -1;//required value, -1 means accept any value
	bool is_interupt = false;//mechanic is ignored if player has stability
	bool is_multihit = true;//mechanic is listed once if it hits multiple times within [frequency_player] ms
	bool target_is_dst = true;//relevant player for mechanic is the destination of cbtevent (setting this to false makes the relevant player the source of cbtevent)
	bool fail_if_hit = true;//mechanic is "failed" if hit (setting this to false makes it neutral in chart)
	bool valid_if_down = false;//mechanic counts if player is in down-state

	/*
	If the attack is successfully evaded/blocked/invulned, arcdps will say such.
	If the attack cannot actually be evaded/blocked/invulned, such a combat event will never happen and it won't matter.
	The following flags are for mechanics where you can avoid the damage, but still cause something bad.
	These flags should be true by default unless a mechanic is quite special.
	*/
	bool can_evade = true;
	bool can_block = true;
	bool can_invuln = true;

	int verbosity = verbosity_all;//if mechanic should be displayed in the log, chart, or everywhere

	Mechanic() noexcept;
	Mechanic(std::string new_name, std::initializer_list<uint32_t> new_ids, Boss* new_boss, bool new_fail_if_hit, bool new_valid_if_down, int new_verbosity,
		bool new_is_interupt, bool new_is_multihit, int new_target_location,
		uint64_t new_frequency_player, uint64_t new_frequency_global, int32_t new_overstack_value, int32_t new_value,
		uint8_t new_is_activation, uint8_t new_is_buffremove,
		bool new_can_evade, bool new_can_block, bool new_can_invuln,
		bool(*new_special_requirement)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player),
		int64_t(*new_special_value)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player),
			std::string new_name_internal, std::string new_description);

	int64_t isValidHit(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst);

	std::string getIniName();
	std::string getChartName();

    bool (*special_requirement)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
    int64_t (*special_value)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);

    Mechanic setName(std::string const new_name) {this->name = new_name; this->name_chart = this->getChartName(); return *this;}
    Mechanic setNameInternal(std::string const new_name_internal) {this->name_internal = new_name_internal; return *this;}
    Mechanic setDescription(std::string const new_description) {this->description = new_description; return *this;}
	Mechanic setIds(std::initializer_list<uint32_t> const new_ids) { std::copy(new_ids.begin(), new_ids.end(), this->ids); this->ids_size = new_ids.size(); return *this; }
    Mechanic setBoss(Boss* const new_boss) {this->boss = new_boss; this->name_chart = this->getChartName(); return *this;}
    Mechanic setFrequencyPlayer(uint64_t const new_frequency_player) {this->frequency_player = new_frequency_player; return *this;}
    Mechanic setFrequencyGlobal(uint64_t const new_frequency_global) {this->frequency_global = new_frequency_global; return *this;}
    Mechanic setIsActivation(uint8_t const new_is_activation) {this->is_activation = new_is_activation; return *this;}
    Mechanic setIsBuffremove(uint8_t const new_is_buffremove) {this->is_buffremove = new_is_buffremove; return *this;}
	Mechanic setOverstackValue(int32_t const new_overstack_value) { this->overstack_value = new_overstack_value; return *this; }
	Mechanic setValue(int32_t const new_value) { this->value = new_value; return *this; }
    Mechanic setIsInterupt(bool const new_is_interupt) {this->is_interupt = new_is_interupt; return *this;}
    Mechanic setIsMultihit(bool const new_is_multihit) {this->is_multihit = new_is_multihit; return *this;}
    Mechanic setTargetIsDst(bool const new_target_is_dst) {this->target_is_dst = new_target_is_dst; return *this;}
    Mechanic setFailIfHit(bool const new_fail_if_hit) {this->fail_if_hit = new_fail_if_hit; return *this;}
    Mechanic setValidIfDown(bool const new_valid_if_down) {this->valid_if_down = new_valid_if_down; return *this;}
	Mechanic setCanEvade(bool const new_can_evade) { this->can_evade = new_can_evade; return *this; }
	Mechanic setCanBlock(bool const new_can_block) { this->can_block = new_can_block; return *this; }
	Mechanic setCanInvuln(bool const new_can_invuln) { this->can_invuln = new_can_invuln; return *this; }
	Mechanic setVerbosity(int const new_verbosity) { this->verbosity = new_verbosity; return *this; }

    Mechanic setSpecialRequirement(bool (*new_special_requirement)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)) {this->special_requirement = new_special_requirement; return *this;}
    Mechanic setSpecialReturnValue(int64_t(*new_special_value)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)) {this->special_value = new_special_value; return *this;}

	bool operator==(Mechanic* other_mechanic);
};

bool requirementDefault(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
bool requirementDhuumSnatch(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
bool requirementBuffApply(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
bool requirementKcCore(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
bool requirementShTdCc(const Mechanic & current_mechanic, cbtevent * ev, ag * ag_src, ag * ag_dst, Player * player_src, Player * player_dst, Player * current_player);
bool requirementCaveEyeCc(const Mechanic & current_mechanic, cbtevent * ev, ag * ag_src, ag * ag_dst, Player * player_src, Player * player_dst, Player * current_player);
bool requirementDhuumMessenger(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);

int64_t valueDefault(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
int64_t valueDhuumShackles(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);

struct DeimosOil
{
	uint16_t id = 0;
	uint64_t first_touch_time = 0;
	uint64_t last_touch_time = 0;
};

bool requirementDeimosOil(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player * current_player);

bool requirementOnSelf(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player * current_player);

std::vector<Mechanic>& getMechanics();
