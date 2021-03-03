#pragma once

#include <string>
#include <vector>
#include <utility>

#include "arcdps_datastructures.h"
#include "player.h"
#include "skill_ids.h"
#include "bosses.h"
#include "helpers.h"

extern bool has_logged_mechanic;

const unsigned int max_ids_per_mechanic = 10;

enum class Verbosity : int
{
	None = 0,
	Chart = 1 << 0,
	Log = 1 << 1,
	All = (Chart | Log),
};

enum class TargetLocation : bool
{
	Source = false,
	Destination = true,
};

class Mechanic
{
private:
	Boss* boss;//required boss, ignored if null
	std::string name; //name of mechanic

public:
	uint32_t ids[max_ids_per_mechanic]; //skill ids;
	size_t ids_size;

    std::string name_internal; //name of skill in skilldef
    std::string name_chart; //name in chart (boss name - mechanic name)
    std::string name_ini; //name used for ini saving
	std::string description; //detailed description of what the mechanic is
    uint64_t frequency_player; //minimum time between instances of this mechanic per player(ms)
    uint64_t frequency_global; //minimum time between instances of this mechanic globally(ms)
    uint64_t last_hit_time; //time of last instance of mechanic
	uint8_t is_activation; //required is_activation type from cbtevent
	uint8_t is_buffremove; //required is_buffremove type from cbtevent
	int32_t overstack_value; //required overstack value, -1 means accept any value
	int32_t value; //required value
    bool is_interupt; //mechanic is ignored if player has stability
    bool is_multihit; //mechanic is listed once if it hits multiple times within [frequency_player] ms
    TargetLocation target_is_dst; //relevant player for mechanic is the destination of cbtevent (setting this to false makes the relevant player the source of cbtevent)
    bool fail_if_hit; //mechanic is "failed" if hit (setting this to false makes it neutral in chart)
    bool valid_if_down; //mechanic counts if player is in down-state

	/*
	If the attack is successfully evaded/blocked/invulned, arcdps will say such.
	If the attack cannot actually be evaded/blocked/invulned, such a combat event will never happen and it won't matter.
	The following flags are for mechanics where you can avoid the damage, but still cause something bad.
	These flags should be true by default unless a mechanic is quite special.
	*/
	bool can_evade;
	bool can_block;
	bool can_invuln;

	Verbosity verbosity;//if mechanic should be displayed in the log, chart, or everywhere

	std::string generateIniName();

	Mechanic(Boss* new_boss,
		std::string new_name,
		std::initializer_list<uint32_t> new_ids,
		std::string new_description,
		bool new_fail_if_hit,
		bool new_is_interupt,
		bool new_is_multihit,
		TargetLocation new_target_location,
		uint64_t new_frequency_player,
		uint64_t new_frequency_global,
		uint8_t new_is_activation,
		uint8_t new_is_buffremove,
		bool new_can_evade,
		bool new_can_block,
		bool new_can_invuln,
		bool(Mechanic::*new_special_requirement)(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player),
		int64_t(Mechanic::*new_special_value)(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player),
		std::string new_name_internal,
		bool new_valid_if_down,
		int32_t new_overstack_value,
		int32_t new_value,
		Verbosity new_verbosity);

	int64_t isValidHit(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst);

	std::string getIniName();
	std::string getChartName();

	//TODO: make read-only? const?
	Boss* getBoss() { return this->boss; };
	const std::string getName() { return this->name; };

    bool (Mechanic::*special_requirement)(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
    int64_t (Mechanic::*special_value)(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);

    Mechanic setNameInternal(std::string const new_name_internal) {this->name_internal = new_name_internal; return *this;}
    Mechanic setDescription(std::string const new_description) {this->description = new_description; return *this;}
    Mechanic setBoss(Boss* const new_boss) {this->boss = new_boss; this->name_chart = this->getChartName(); return *this;}
    Mechanic setFrequencyPlayer(uint64_t const new_frequency_player) {this->frequency_player = new_frequency_player; return *this;}
    Mechanic setFrequencyGlobal(uint64_t const new_frequency_global) {this->frequency_global = new_frequency_global; return *this;}
    Mechanic setIsActivation(uint8_t const new_is_activation) {this->is_activation = new_is_activation; return *this;}
    Mechanic setIsBuffremove(uint8_t const new_is_buffremove) {this->is_buffremove = new_is_buffremove; return *this;}
	Mechanic setOverstackValue(int32_t const new_overstack_value) { this->overstack_value = new_overstack_value; return *this; }
	Mechanic setValue(int32_t const new_value) { this->value = new_value; return *this; }
    Mechanic setIsInterupt(bool const new_is_interupt) {this->is_interupt = new_is_interupt; return *this;}
    Mechanic setIsMultihit(bool const new_is_multihit) {this->is_multihit = new_is_multihit; return *this;}
    Mechanic setTargetIsDst(TargetLocation const new_target_is_dst) {this->target_is_dst = new_target_is_dst; return *this;}
    Mechanic setFailIfHit(bool const new_fail_if_hit) {this->fail_if_hit = new_fail_if_hit; return *this;}
    Mechanic setValidIfDown(bool const new_valid_if_down) {this->valid_if_down = new_valid_if_down; return *this;}
	Mechanic setCanEvade(bool const new_can_evade) { this->can_evade = new_can_evade; return *this; }
	Mechanic setCanBlock(bool const new_can_block) { this->can_block = new_can_block; return *this; }
	Mechanic setCanInvuln(bool const new_can_invuln) { this->can_invuln = new_can_invuln; return *this; }
	Mechanic setVerbosity(Verbosity const new_verbosity) { this->verbosity = new_verbosity; return *this; }

    Mechanic setSpecialRequirement(bool (Mechanic::*new_special_requirement)(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)) {special_requirement = new_special_requirement; return *this;}
    Mechanic setSpecialReturnValue(int64_t(Mechanic::*new_special_value)(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)) {special_value = new_special_value; return *this;}

	bool operator==(Mechanic* other_mechanic);

	bool requirementDefault(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
	bool requirementDhuumSnatch(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
	bool requirementBuffApply(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
	bool requirementKcCore(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
	bool requirementShTdCc(cbtevent * ev, ag * ag_src, ag * ag_dst, Player * player_src, Player * player_dst, Player * current_player);
	bool requirementCaveEyeCc(cbtevent * ev, ag * ag_src, ag * ag_dst, Player * player_src, Player * player_dst, Player * current_player);
	bool requirementDhuumMessenger(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player);
	bool requirementDeimosOil(cbtevent* ev, ag* ag_src, ag* ag_dst, Player* player_src, Player* player_dst, Player* current_player);
	bool requirementOnSelf(cbtevent* ev, ag* ag_src, ag* ag_dst, Player* player_src, Player* player_dst, Player* current_player);
	int64_t valueDefault(cbtevent* ev, ag* ag_src, ag* ag_dst, Player* player_src, Player* player_dst, Player* current_player);
	int64_t valueDhuumShackles(cbtevent* ev, ag* ag_src, ag* ag_dst, Player* player_src, Player* player_dst, Player* current_player);

};


struct DeimosOil
{
	uint16_t id = 0;
	uint64_t first_touch_time = 0;
	uint64_t last_touch_time = 0;
};


std::vector<Mechanic>& getMechanics();
