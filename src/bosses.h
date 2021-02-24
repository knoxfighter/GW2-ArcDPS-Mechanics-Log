#pragma once

#include <stdint.h>
#include <vector>
#include <list>
#include <string>

class Boss
{
public:
	std::vector<uint32_t> ids = {};//npc id (from ag->prof)
	std::string name = "";
	int64_t timer = 0;//enrage timer
	uint64_t health = 0;
	uint16_t pulls = 0;

	Boss::Boss(std::string const p_name, std::vector<uint32_t> p_ids, int64_t p_timer, uint64_t p_health);

	bool hasId(uint32_t new_id);

	Boss setIds(std::initializer_list<uint32_t> const new_ids) { this->ids = std::vector<uint32_t>(new_ids); return *this; }
	Boss setName(std::string const new_name) { this->name = new_name; return *this; }
	Boss setTimer(int64_t const new_timer) { this->timer = new_timer; return *this; }
	Boss setHealth(uint64_t const new_health) { this->health = new_health; return *this; }

	bool operator==(Boss* other_boss);
};

extern Boss boss_generic;

//Raids

//W1
extern Boss boss_vg;
extern Boss boss_gors;
extern Boss boss_sab;

//W2
extern Boss boss_sloth;
extern Boss boss_trio;
extern Boss boss_matti;

//W3
//TODO: Add escort boss
extern Boss boss_kc;
extern Boss boss_xera;

//W4
extern Boss boss_cairn;
extern Boss boss_mo;
extern Boss boss_sam;
extern Boss boss_deimos;

//W5
extern Boss boss_sh;
//TODO: Add soul river boss
extern Boss boss_soul_eater;
extern Boss boss_ice_king;
extern Boss boss_cave;
extern Boss boss_dhuum;

//W6
extern Boss boss_ca;
extern Boss boss_largos;
extern Boss boss_qadim;

//W7
extern Boss boss_adina;
extern Boss boss_sabir;
extern Boss boss_qadim2;

//Fractals
extern Boss boss_fotm_generic;

//Nightmare
extern Boss boss_mama;
extern Boss boss_siax;
extern Boss boss_ensolyss;

//Shattered Observatory
extern Boss boss_skorvald;
extern Boss boss_artsariiv;
extern Boss boss_arkk;

//Sunqua Peak
extern Boss boss_ai;

//Strike Missions
extern Boss boss_icebrood_construct;
extern Boss boss_voice_and_claw;
extern Boss boss_fraenir;
extern Boss boss_boneskinner;
extern Boss boss_whisper;
extern Boss boss_coldwar;

extern std::list<Boss*> bosses;
