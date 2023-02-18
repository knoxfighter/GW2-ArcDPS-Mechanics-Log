#include "mechanics.h"

bool has_logged_mechanic = false;

Mechanic::Mechanic() noexcept
{
	special_requirement = requirementDefault;
	special_value = valueDefault;
}

Mechanic::Mechanic(std::string new_name, std::initializer_list<uint32_t> new_ids, Boss* new_boss, bool new_fail_if_hit, bool new_valid_if_down, int new_verbosity,
	bool new_is_interupt, bool new_is_multihit, int new_target_location,
	uint64_t new_frequency_player, uint64_t new_frequency_global, int32_t new_overstack_value, int32_t new_value,
	uint8_t new_is_activation, uint8_t new_is_buffremove,
	bool new_can_evade, bool new_can_block, bool new_can_invuln,
	bool(*new_special_requirement)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player),
	int64_t(*new_special_value)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player),
	std::string new_name_internal, std::string new_description)
{
	name = new_name;

	std::copy(new_ids.begin(), new_ids.end(), ids);
	ids_size = new_ids.size();

	boss = new_boss;
	fail_if_hit = new_fail_if_hit;
	valid_if_down = new_valid_if_down;
	verbosity = new_verbosity;

	is_interupt = new_is_interupt;
	is_multihit = new_is_multihit;
	target_is_dst = new_target_location;

	frequency_player = new_frequency_player;
	frequency_global = new_frequency_global;

	overstack_value = new_overstack_value;
	value = new_value;

	is_activation = new_is_activation;
	is_buffremove = new_is_buffremove;

	can_evade = new_can_evade;
	can_block = new_can_block;
	can_invuln = new_can_invuln;

	special_requirement = new_special_requirement;
	special_value = new_special_value;

	name_internal = new_name_internal;
	description = new_description;

	name_chart = (new_boss ? new_boss->name : "")
		+ " - " + new_name;
	name_ini = getIniName();//TODO: replace this with the code from getIniName() once all mechanic definitions use new style
}

int64_t Mechanic::isValidHit(cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst)
{
	uint16_t index = 0;
	bool correct_id = false;
	Player* current_player = nullptr;

	if (!ev) return false;
	if (!player_src && !player_dst) return false;

	if (can_block && ev->result == CBTR_BLOCK) return false;
	if (can_evade && ev->result == CBTR_EVADE) return false;
	if (can_invuln && ev->result == CBTR_ABSORB) return false;

	if (!verbosity) return false;

	for (index = 0; index < ids_size; index++)
	{
		if (ev->skillid == this->ids[index])
		{
			correct_id = true;
			break;
		}
	}

	if (!correct_id
		&& ids_size > 0) return false;

	if (frequency_global != 0
		&& ev->time < last_hit_time + frequency_global - ms_per_tick)
	{
		return false;
	}

	if (ev->is_buffremove != is_buffremove)
	{
		return false;
	}

	if (is_activation)
	{//Normal and quickness activations are interchangable
		if (is_activation == ACTV_NORMAL
			|| is_activation == ACTV_QUICKNESS)
		{
			if (ev->is_activation != ACTV_NORMAL
				&& ev->is_activation != ACTV_QUICKNESS)
			{
				return false;
			}
		}
	}

	if (is_buffremove//TODO: this check is wrong. overstack does not require buffremove
		&& overstack_value >= 0
		&& overstack_value != ev->overstack_value)
	{
		return false;
	}

	if (value != -1
		&& ev->value != value)
	{
		return false;
	}

	if (target_is_dst)
	{
		current_player = player_dst;
	}
	else
	{
		current_player = player_src;
	}

	if (!current_player) return false;

	if (!valid_if_down && current_player->is_downed) return false;

	if (is_interupt && current_player->last_stab_time > ev->time) return false;

	if (!special_requirement(*this, ev, ag_src, ag_dst, player_src, player_dst, current_player)) return false;

	last_hit_time = ev->time;

	return special_value(*this, ev, ag_src, ag_dst, player_src, player_dst, current_player);
}

std::string Mechanic::getIniName()
{
	return std::to_string(ids[0])
		+ " - " + (boss ? boss->name : "")
		+ " - " + name;
}

std::string Mechanic::getChartName()
{
	return (boss ? boss->name : "")
		+ " - " + name;
}

bool Mechanic::operator==(Mechanic* other_mechanic)
{
	return other_mechanic && ids[0] == other_mechanic->ids[0];
}

bool requirementDefault(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	return true;
}

bool requirementDhuumSnatch(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	static std::list<std::pair<uint16_t, uint64_t>> players_snatched;//pair is <instance id,last snatch time>

	for (auto current_pair = players_snatched.begin(); current_pair != players_snatched.end(); ++current_pair)
	{
		//if player has been snatched before and is in tracking
		if (ev->dst_instid == current_pair->first)
		{
			if ((current_pair->second + current_mechanic.frequency_player) > ev->time)
			{
				current_pair->second = ev->time;
				return false;
			}
			else
			{
				current_pair->second = ev->time;
				return true;
			}
		}
	}

	//if player not seen before
	players_snatched.push_back(std::pair<uint16_t, uint64_t>(ev->dst_instid, ev->time));
	return true;
}

bool requirementBuffApply(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player * current_player)
{
	return ev
		&& ev->buff
		&& ev->buff_dmg == 0;
}

bool requirementKcCore(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	if (!ev) return false;

	//need player as src and agent (core) as dst
	if (!player_src) return false;
	if (!ag_dst) return false;

	//must be physical hit
	if (ev->is_statechange) return false;
	if (ev->is_activation) return false;
	if (ev->is_buffremove) return false;
	if (ev->buff) return false;

	//must be hitting kc core
	if (ag_dst->prof != 16261) return false;

	return true;
}

bool requirementShTdCc(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	if (!ev) return false;

	//need player as src and agent (TD) as dst
	if (!player_src) return false;
	if (!ag_dst) return false;

	//must be buff apply
	if (ev->is_statechange) return false;
	if (ev->is_activation) return false;
	if (ev->is_buffremove) return false;
	if (!ev->buff) return false;
	if (ev->buff_dmg) return false;

	//must be hitting a tormented dead
	if (ag_dst->prof != 19422) return false;

	return true;
}

bool requirementCaveEyeCc(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	if (!ev) return false;

	//need player as src and agent (Eye) as dst
	if (!player_src) return false;
	if (!ag_dst) return false;

	//must be buff apply
	if (ev->is_statechange) return false;
	if (ev->is_activation) return false;
	if (ev->is_buffremove) return false;
	if (!ev->buff) return false;
	if (ev->buff_dmg) return false;

	//must be hitting an eye
	if (ag_dst->prof != 0x4CC3
		&& ag_dst->prof != 0x4D84) return false;

	return true;
}

bool requirementDhuumMessenger(const Mechanic & current_mechanic, cbtevent * ev, ag * ag_src, ag * ag_dst, Player * player_src, Player * player_dst, Player * current_player)
{
	static std::list<uint16_t> messengers;
	static std::mutex messengers_mtx;

	if (!ev) return false;

	//need player as src and agent (messenger) as dst
	if (!player_src) return false;
	if (!ag_dst) return false;

	//must be physical hit
	if (ev->is_statechange) return false;
	if (ev->is_activation) return false;
	if (ev->is_buffremove) return false;
	if (ev->buff) return false;

	//must be hitting a messenger
	if (ag_dst->prof != 19807) return false;

	const auto new_messenger = ev->dst_instid;

	std::lock_guard<std::mutex> lg(messengers_mtx);

	auto it = std::find(messengers.begin(), messengers.end(), new_messenger);

	if (it != messengers.end())
	{
		return false;
	}

	messengers.push_front(new_messenger);
	return true;
}

bool requirementDeimosOil(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	static const uint16_t max_deimos_oils = 3;
	static DeimosOil deimos_oils[max_deimos_oils];

	DeimosOil* current_oil = nullptr;
	DeimosOil* oldest_oil = &deimos_oils[0];

	//find if the oil is already tracked
	for (auto index = 0; index < max_deimos_oils; index++)
	{
		if (deimos_oils[index].last_touch_time < oldest_oil->last_touch_time)//find oldest oil
		{
			oldest_oil = &deimos_oils[index];
		}
		if (deimos_oils[index].id == ev->src_instid)//if oil is already known
		{
			current_oil = &deimos_oils[index];
		}
	}

	//if oil is new
	if (!current_oil)
	{
		current_oil = oldest_oil;
		current_oil->id = ev->src_instid;
		current_oil->first_touch_time = ev->time;
		current_oil->last_touch_time = ev->time;
		return true;
	}
	else
	{//oil is already known
		current_oil->last_touch_time = ev->time;
		if ((ev->time - current_oil->last_touch_time) > current_mechanic.frequency_player)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool requirementOnSelf(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	return ev->src_instid == ev->dst_instid;
}

bool requirementOnSelfRevealedInHarvestTemple(const Mechanic& current_mechanic, cbtevent* ev,
							   ag* ag_src, ag* ag_dst, Player* player_src,
							   Player* player_dst, Player* current_player)
{
	
	if (!ev) return false;
	// In Harvest Temple
	if (!current_player->current_log_npc || !current_mechanic.boss->hasId(*current_player->current_log_npc)) return false;
	// Applying to self
	if (ev->src_instid != ev->dst_instid) return false;
	// Applying, not removing
	if (ev->is_buffremove) return false;
	// Applying buff
	if (!ev->buff || ev->buff_dmg != 0) return false;
	return true;
}

bool requirementSpecificBoss(const Mechanic& current_mechanic, cbtevent* ev,
                             ag* ag_src, ag* ag_dst, Player* player_src,
                             Player* player_dst, Player* current_player)
{
	if (!current_player->current_log_npc) return false;
	if (!current_mechanic.boss) return false;
	return current_mechanic.boss->hasId(*current_player->current_log_npc);
}

int64_t valueDefault(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	return 1;
}

int64_t valueDhuumShackles(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player * current_player)
{
	return (30000 - ev->value) / 1000;
}

std::vector<Mechanic>& getMechanics()
{
	static std::vector<Mechanic>* mechanics =
		new std::vector<Mechanic>
	{
		//Vale Guardian
		Mechanic("was teleported",{31392,31860},&boss_vg,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Unstable Magic Spike",""),
		//I'm not sure why this mechanic has 4 ids, but it appears to for some reason
		//all these ids are for when 4 people are in the green circle
		//it appears to be a separate id for the 90% hp blast when <4 people are in the green
		//all 4 ids are called "Distributed Magic"
		Mechanic("stood in the green circle",{31340,31391,31529,31750},&boss_vg,false,false,verbosity_chart,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"Distributed Magic",""),

		//Gorseval
		Mechanic().setName("was slammed").setIds({MECHANIC_GORS_SLAM}).setIsInterupt(true).setBoss(&boss_gors),
		Mechanic().setName("was egged").setIds({MECHANIC_GORS_EGG}).setBoss(&boss_gors),
		Mechanic().setName("touched an orb").setIds({MECHANIC_GORS_ORB}).setBoss(&boss_gors).setSpecialRequirement(requirementBuffApply),
		
		//Sabetha
		Mechanic().setName("got a sapper bomb").setIds({MECHANIC_SAB_SAPPER_BOMB}).setFailIfHit(false).setValidIfDown(true).setBoss(&boss_sab),
		Mechanic().setName("got a time bomb").setIds({MECHANIC_SAB_TIME_BOMB}).setFailIfHit(false).setValidIfDown(true).setBoss(&boss_sab),
		Mechanic().setName("stood in cannon fire").setIds({MECHANIC_SAB_CANNON}).setBoss(&boss_sab),
		//Mechanic().setName("touched the flame wall").setIds({MECHANIC_SAB_FLAMEWALL}).setBoss(&boss_sab),

		//Slothasaur
		Mechanic().setName("was hit with tantrum").setIds({MECHANIC_SLOTH_TANTRUM}).setBoss(&boss_sloth),
		Mechanic().setName("got a bomb").setIds({MECHANIC_SLOTH_BOMB}).setFailIfHit(false).setFrequencyPlayer(6000).setBoss(&boss_sloth),
		Mechanic().setName("stood in bomb aoe").setIds({MECHANIC_SLOTH_BOMB_AOE}).setVerbosity(verbosity_chart).setBoss(&boss_sloth),
		Mechanic().setName("was hit by flame breath").setIds({MECHANIC_SLOTH_FLAME_BREATH}).setBoss(&boss_sloth),
		Mechanic().setName("was hit by shake").setIds({MECHANIC_SLOTH_SHAKE}).setBoss(&boss_sloth),
		Mechanic().setName("is fixated").setIds({MECHANIC_SLOTH_FIXATE}).setFailIfHit(false).setBoss(&boss_sloth),

		//Bandit Trio
		Mechanic("threw a beehive",{34533},&boss_trio,false,false,verbosity_all,false,false,target_location_src,0,0,-1,-1,ACTV_NORMAL,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"Beehive",""),
		Mechanic("threw an oil keg",{34471},&boss_trio,false,false,verbosity_all,false,false,target_location_src,0,0,-1,-1,ACTV_NORMAL,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"Throw",""),

		//Matthias
		//Mechanic().setName("was hadoukened").setIds({MECHANIC_MATT_HADOUKEN_HUMAN,MECHANIC_MATT_HADOUKEN_ABOM}).setBoss(&boss_matti),
		Mechanic().setName("reflected shards").setIds({MECHANIC_MATT_SHARD_HUMAN,MECHANIC_MATT_SHARD_ABOM}).setTargetIsDst(false).setBoss(&boss_matti),
		Mechanic().setName("got a bomb").setIds({MECHANIC_MATT_BOMB}).setFailIfHit(false).setFrequencyPlayer(12000).setBoss(&boss_matti),
		Mechanic().setName("got a corruption").setIds({MECHANIC_MATT_CORRUPTION}).setFailIfHit(false).setBoss(&boss_matti),
		Mechanic().setName("is sacrificed").setIds({MECHANIC_MATT_SACRIFICE}).setFailIfHit(false).setBoss(&boss_matti),
		Mechanic("touched a ghost",{34413},&boss_matti,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Surrender",""),
		//Mechanic("touched an icy patch",{26766},&boss_matti,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,10000,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Slow",""), //look for Slow application with 10 sec duration. Disabled because some mob in Istan applies the same duration of slow
		Mechanic("stood in tornado",{34466},&boss_matti,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Fiery Vortex",""),
		Mechanic("stood in storm cloud",{34543},&boss_matti,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Thunder",""),

		//Keep Construct
		Mechanic().setName("is fixated").setIds({MECHANIC_KC_FIXATE}).setFailIfHit(false).setBoss(&boss_kc),
		//Mechanic().setName("is west fixated").setIds({MECHANIC_KC_FIXATE_WEST}).setFailIfHit(false).setBoss(&boss_kc),
		Mechanic().setName("touched the core").setFailIfHit(false).setTargetIsDst(false).setFrequencyPlayer(8000).setBoss(&boss_kc).setSpecialRequirement(requirementKcCore),
		Mechanic("was squashed",{35086},&boss_kc,true,false,verbosity_all,true,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Tower Drop",""),
		Mechanic("stood in donut",{35137,34971,35086},&boss_kc,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Phantasmal Blades",""),
		
		//Xera
		Mechanic("stood in red half",{34921},&boss_xera,true,false,verbosity_all,false,true,target_location_dst,4000,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,true,requirementDefault,valueDefault,"TODO:check internal name",""),
		Mechanic().setName("has magic").setIds({MECHANIC_XERA_MAGIC}).setFailIfHit(false).setValidIfDown(true).setValue(15000).setBoss(&boss_xera),
		Mechanic().setName("used magic").setIds({MECHANIC_XERA_MAGIC_BUFF}).setFailIfHit(false).setTargetIsDst(false).setFrequencyGlobal(12000).setValidIfDown(true).setBoss(&boss_xera).setSpecialRequirement(requirementOnSelf).setVerbosity(0),
		Mechanic().setName("triggered an orb").setIds({MECHANIC_XERA_ORB}).setBoss(&boss_xera),
		Mechanic().setName("stood in an orb aoe").setIds({MECHANIC_XERA_ORB_AOE}).setFrequencyPlayer(1000).setVerbosity(verbosity_chart).setBoss(&boss_xera),
		Mechanic().setName("was teleported").setIds({MECHANIC_XERA_PORT}).setVerbosity(verbosity_chart).setBoss(&boss_xera),

		//Cairn
		Mechanic().setName("was teleported").setIds({MECHANIC_CAIRN_TELEPORT}).setBoss(&boss_cairn),
		Mechanic().setName("was slapped").setIds({MECHANIC_CAIRN_SWEEP}).setBoss(&boss_cairn).setIsInterupt(true),
		//Mechanic().setName("reflected shards").setIds({MECHANIC_CAIRN_SHARD}).setTargetIsDst(false).setBoss(&boss_cairn),
		Mechanic().setName("missed a green circle").setIds({MECHANIC_CAIRN_GREEN_A,MECHANIC_CAIRN_GREEN_B,MECHANIC_CAIRN_GREEN_C,MECHANIC_CAIRN_GREEN_D,MECHANIC_CAIRN_GREEN_E,MECHANIC_CAIRN_GREEN_F}).setIsInterupt(true).setBoss(&boss_cairn),
		
		//Samarog
		Mechanic().setName("was shockwaved").setIds({MECHANIC_SAM_SHOCKWAVE}).setIsInterupt(true).setBoss(&boss_sam),
		Mechanic().setName("was horizontally slapped").setIds({MECHANIC_SAM_SLAP_HORIZONTAL}).setIsInterupt(true).setBoss(&boss_sam),
		Mechanic().setName("was vertically smacked").setIds({MECHANIC_SAM_SLAP_VERTICAL}).setIsInterupt(true).setBoss(&boss_sam),
		Mechanic().setName("is fixated").setIds({MECHANIC_SAM_FIXATE_SAM}).setFailIfHit(false).setBoss(&boss_sam),
		Mechanic().setName("has big green").setIds({MECHANIC_SAM_GREEN_BIG}).setFailIfHit(false).setBoss(&boss_sam),
		Mechanic().setName("has small green").setIds({MECHANIC_SAM_GREEN_SMALL}).setFailIfHit(false).setBoss(&boss_sam),

		//Deimos
		Mechanic("touched an oil",{37716},&boss_deimos,true,false,verbosity_all,false,true,target_location_dst,5000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDeimosOil,valueDefault,"Rapid Decay",""),
		Mechanic().setName("was smashed").setIds({MECHANIC_DEIMOS_SMASH,MECHANIC_DEIMOS_SMASH_INITIAL,MECHANIC_DEIMOS_SMASH_END_A,MECHANIC_DEIMOS_SMASH_END_B}).setBoss(&boss_deimos),
		Mechanic().setName("closed a tear").setIds({MECHANIC_DEIMOS_TEAR}).setFailIfHit(false).setBoss(&boss_deimos),
		Mechanic("has the teleport",{37730},&boss_deimos,false,true,verbosity_all,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"Chosen by Eye of Janthir",""),
		Mechanic("was teleported",{38169},&boss_deimos,false,true,verbosity_chart,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"",""),

		//Soulless Horror
		Mechanic().setName("stood in inner ring").setIds({MECHANIC_HORROR_DONUT_INNER}).setVerbosity(verbosity_chart).setBoss(&boss_sh),
		Mechanic().setName("stood in outer ring").setIds({MECHANIC_HORROR_DONUT_OUTER}).setVerbosity(verbosity_chart).setBoss(&boss_sh),
		Mechanic().setName("stood in torment aoe").setIds({MECHANIC_HORROR_GOLEM_AOE}).setBoss(&boss_sh),
		Mechanic().setName("stood in pie slice").setIds({MECHANIC_HORROR_PIE_4_A,MECHANIC_HORROR_PIE_4_B}).setVerbosity(verbosity_chart).setBoss(&boss_sh),
		Mechanic().setName("touched a scythe").setIds({MECHANIC_HORROR_SCYTHE}).setBoss(&boss_sh),
		Mechanic().setName("took fixate").setIds({MECHANIC_HORROR_FIXATE}).setFailIfHit(false).setVerbosity(verbosity_chart).setBoss(&boss_sh),
		Mechanic().setName("was debuffed").setIds({MECHANIC_HORROR_DEBUFF}).setFailIfHit(false).setVerbosity(verbosity_chart).setBoss(&boss_sh),
		Mechanic("CCed a tormented dead",{872,833,31465},&boss_sh,true,true,verbosity_all,false,true,target_location_src,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,requirementShTdCc,valueDefault,"Stun, Daze, Temporal stasis",""),

		//Statues
		Mechanic().setName("was puked on").setIds({MECHANIC_EATER_PUKE}).setFrequencyPlayer(3000).setVerbosity(verbosity_chart).setBoss(&boss_soul_eater),
		Mechanic().setName("stood in web").setIds({MECHANIC_EATER_WEB}).setFrequencyPlayer(3000).setVerbosity(verbosity_chart).setBoss(&boss_soul_eater),
		Mechanic().setName("got an orb").setIds({MECHANIC_EATER_ORB}).setFrequencyPlayer(ms_per_tick).setFailIfHit(false).setBoss(&boss_soul_eater),
		Mechanic().setName("threw an orb").setNameInternal("Reclaimed Energy").setIds({47942}).setTargetIsDst(false).setIsActivation(ACTV_NORMAL).setFailIfHit(false).setBoss(&boss_soul_eater),
		Mechanic("got a green",{47013},&boss_ice_king,false,true,verbosity_chart,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Hailstorm",""),
		Mechanic("CCed an eye",{872},&boss_cave,false,true,verbosity_all,false,false,target_location_src,0,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementCaveEyeCc,valueDefault,"Stun",""),

		//Dhuum
		Mechanic().setName("touched a messenger").setIds({MECHANIC_DHUUM_GOLEM}).setBoss(&boss_dhuum),
		Mechanic().setName("is shackled").setIds({MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false).setTargetIsDst(false).setBoss(&boss_dhuum),
		Mechanic().setName("is shackled").setIds({MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false).setBoss(&boss_dhuum),
		//Mechanic().setName("popped shackles").setIds({MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false).setIsBuffremove(CBTB_MANUAL).setTargetIsDst(false).setSpecialValue(valueDhuumShackles).setBoss(&boss_dhuum),
		//Mechanic().setName("popped shackles").setIds({MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false).setIsBuffremove(CBTB_MANUAL).setSpecialValue(valueDhuumShackles).setBoss(&boss_dhuum),
		Mechanic().setName("has affliction").setIds({MECHANIC_DHUUM_AFFLICTION}).setFrequencyPlayer(13000 + ms_per_tick).setFailIfHit(false).setValidIfDown(true).setBoss(&boss_dhuum),
		Mechanic("took affliction damage",{48121},&boss_dhuum,false,true,verbosity_chart,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"Arcing Affliction",""),
		Mechanic().setName("stood in a crack").setIds({MECHANIC_DHUUM_CRACK}).setBoss(&boss_dhuum),
		Mechanic().setName("stood in a poison mark").setIds({MECHANIC_DHUUM_MARK}).setVerbosity(verbosity_chart).setBoss(&boss_dhuum),
		Mechanic().setName("was sucked center").setIds({MECHANIC_DHUUM_SUCK_AOE}).setBoss(&boss_dhuum),
		Mechanic().setName("stood in dip aoe").setIds({MECHANIC_DHUUM_TELEPORT_AOE}).setBoss(&boss_dhuum),
		//Mechanic().setName("died on green").setIds({MECHANIC_DHUUM_GREEN_TIMER}).setIsBuffremove(CBTB_MANUAL).setOverstackValue(0).setBoss(&boss_dhuum),
		//Mechanic().setName("aggroed a messenger").setNameInternal("").setTargetIsDst(false).setFailIfHit(false).setFrequencyPlayer(0).setValidIfDown(true).setBoss(&boss_dhuum).setSpecialRequirement(requirementDhuumMessenger),
		Mechanic().setName("was snatched").setIds({MECHANIC_DHUUM_SNATCH}).setSpecialRequirement(requirementDhuumSnatch).setBoss(&boss_dhuum),
		//Mechanic().setName("canceled button channel").setIds({MECHANIC_DHUUM_BUTTON_CHANNEL}).setIsActivation(ACTV_CANCEL_CANCEL).setBoss(&boss_dhuum),
		Mechanic().setName("stood in cone").setIds({MECHANIC_DHUUM_CONE}).setBoss(&boss_dhuum),

		//Conjured Amalgamate
		Mechanic().setName("was squashed").setIds({MECHANIC_AMAL_SQUASH}).setIsInterupt(true).setBoss(&boss_ca),
		Mechanic().setName("used a sword").setNameInternal("Conjured Greatsword").setIds({52325}).setTargetIsDst(false).setIsActivation(ACTV_NORMAL).setFailIfHit(false).setBoss(&boss_ca),
		Mechanic().setName("used a shield").setNameInternal("Conjured Protection").setIds({52780}).setTargetIsDst(false).setIsActivation(ACTV_NORMAL).setFailIfHit(false).setBoss(&boss_ca),

		//Twin Largos Assasins
		Mechanic().setName("was shockwaved").setIds({MECHANIC_LARGOS_SHOCKWAVE}).setIsInterupt(true).setBoss(&boss_largos),
		Mechanic().setName("was waterlogged").setIds({MECHANIC_LARGOS_WATERLOGGED}).setVerbosity(verbosity_chart).setValidIfDown(true).setFrequencyPlayer(1).setBoss(&boss_largos),
		Mechanic().setName("was bubbled").setIds({MECHANIC_LARGOS_BUBBLE}).setBoss(&boss_largos),
		Mechanic().setName("has a tidal pool").setIds({MECHANIC_LARGOS_TIDAL_POOL}).setFailIfHit(false).setBoss(&boss_largos),
		Mechanic().setName("stood in geyser").setIds({MECHANIC_LARGOS_GEYSER}).setBoss(&boss_largos),
		Mechanic().setName("was dashed over").setIds({MECHANIC_LARGOS_DASH}).setBoss(&boss_largos),
		Mechanic().setName("had boons stolen").setIds({MECHANIC_LARGOS_BOON_RIP}).setBoss(&boss_largos),
		Mechanic().setName("stood in whirlpool").setIds({MECHANIC_LARGOS_WHIRLPOOL}).setBoss(&boss_largos),

		//Qadim
		Mechanic().setName("was shockwaved").setIds({MECHANIC_QADIM_SHOCKWAVE_A,MECHANIC_QADIM_SHOCKWAVE_B}).setBoss(&boss_qadim),
		Mechanic().setName("stood in arcing fire").setIds({MECHANIC_QADIM_ARCING_FIRE_A,MECHANIC_QADIM_ARCING_FIRE_B,MECHANIC_QADIM_ARCING_FIRE_C}).setVerbosity(verbosity_chart).setBoss(&boss_qadim),
		//Mechanic().setName("stood in giant fireball").setIds({MECHANIC_QADIM_BOUNCING_FIREBALL_BIG_A,MECHANIC_QADIM_BOUNCING_FIREBALL_BIG_B,MECHANIC_QADIM_BOUNCING_FIREBALL_BIG_C}).setBoss(&boss_qadim),
		Mechanic().setName("was teleported").setIds({MECHANIC_QADIM_TELEPORT}).setBoss(&boss_qadim).setValidIfDown(true),
		Mechanic().setName("stood in hitbox").setNameInternal("Sea of Flame").setIds({52461}).setBoss(&boss_qadim),

		//Adina
		Mechanic("was blinded",{56593},&boss_adina,false,false,verbosity_chart,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Radiant Blindness",""),
		Mechanic("looked at eye", {56114}, &boss_adina, false, false, verbosity_all, false, true, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE,true, false, true,requirementDefault, valueDefault, "Diamond Palisade", ""),
		Mechanic("touched pillar ripple",{56558},&boss_adina,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Tectonic Upheaval",""),
		Mechanic("touched a mine",{56141},&boss_adina,true,false,verbosity_all,false,true,target_location_dst,1000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Stalagmites",""),
		//Mechanic("has pillar",{47860},&boss_adina,false,false,verbosity_all,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"",""),//wrong id?
		
		//Sabir
		Mechanic("touched big tornado",{56202},&boss_sabir,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Dire Drafts",""),
		Mechanic("was shockwaved",{56643},&boss_sabir,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,true,requirementDefault,valueDefault,"Unbridled Tempest",""),
		Mechanic("wasn't in bubble",{56372},&boss_sabir,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,requirementDefault,valueDefault,"Fury of the Storm",""),
		Mechanic("was bopped at phase", {56094},&boss_sabir,true,false,verbosity_chart,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Walloping Wind",""),

		//Qadim the Peerless
		Mechanic("is tank",{56510},&boss_qadim2,false,true,verbosity_all,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Fixated",""),
		Mechanic("touched lava", {56180,56378,56541},&boss_qadim2,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Residual Impact, Pylon Debris Field",""),//ids are big,small(CM),pylon
		Mechanic("was struck by small lightning",{56656},&boss_qadim2,true,false,verbosity_chart,false,false,target_location_dst,1000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Brandstorm Lightning",""),		
		Mechanic("was hit by triple lightning",{56527},&boss_qadim2,true,false,verbosity_all,false,false,target_location_dst,1000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Rain of Chaos",""),
		Mechanic("touched arcing line",{56145},&boss_qadim2,true,false,verbosity_all,false,false,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Chaos Called",""),
		Mechanic("was shockwaved",{56134},&boss_qadim2,true,false,verbosity_all,true,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Force of Retaliation",""),
		Mechanic("touched purple rectangle",{56441},&boss_qadim2,true,false,verbosity_chart,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Force of Havoc",""),
		Mechanic("was ran over",{56616},&boss_qadim2,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Battering Blitz",""),
		Mechanic("was sniped",{56332},&boss_qadim2,true,true,verbosity_all,false,false,target_location_dst,100,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Caustic Chaos",""),
		Mechanic("was splashed by sniper",{56543},&boss_qadim2,false,true,verbosity_chart,false,false,target_location_dst,100,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"Caustic Chaos",""),
		//Mechanic("has lightning", {51371},&boss_qadim2,false,true,verbosity_all,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,"",""),

		//Fractals of the Mist Instabilities
		Mechanic().setName("got a flux bomb").setIds({MECHANIC_FOTM_FLUX_BOMB}).setFailIfHit(false).setBoss(&boss_fotm_generic),

		//MAMA
		//Mechanic().setName("vomited on someone").setIds({MECHANIC_NIGHTMARE_VOMIT}).setTargetIsDst(false).setBoss(&boss_fotm_generic),
		Mechanic().setName("was hit by whirl").setIds({MECHANIC_MAMA_WHIRL,MECHANIC_MAMA_WHIRL_NORMAL}).setBoss(&boss_mama),
		Mechanic().setName("was knocked").setIds({MECHANIC_MAMA_KNOCK}).setBoss(&boss_mama),
		Mechanic().setName("was leaped on").setIds({MECHANIC_MAMA_LEAP}).setBoss(&boss_mama),
		Mechanic().setName("stood in acid").setIds({MECHANIC_MAMA_ACID}).setBoss(&boss_mama),
		Mechanic().setName("was smashed by a knight").setIds({MECHANIC_MAMA_KNIGHT_SMASH}).setBoss(&boss_mama),

		//Siax
		Mechanic().setName("stood in acid").setIds({MECHANIC_SIAX_ACID}).setBoss(&boss_siax),

		//Ensolyss
		Mechanic().setName("was ran over").setIds({MECHANIC_ENSOLYSS_LUNGE}).setBoss(&boss_ensolyss),
		Mechanic().setName("was smashed").setIds({MECHANIC_ENSOLYSS_SMASH}).setBoss(&boss_ensolyss),

		//Arkk
		Mechanic().setName("stood in a pie slice").setIds({MECHANIC_ARKK_PIE_A,MECHANIC_ARKK_PIE_B,MECHANIC_ARKK_PIE_C}).setBoss(&boss_arkk),
		//Mechanic().setName("was feared").setIds({MECHANIC_ARKK_FEAR}),
		Mechanic().setName("was smashed").setIds({MECHANIC_ARKK_OVERHEAD_SMASH}).setBoss(&boss_arkk),
		Mechanic().setName("has a bomb").setIds({MECHANIC_ARKK_BOMB}).setFailIfHit(false).setValidIfDown(true).setBoss(&boss_arkk),//TODO Add BOSS_ARTSARIIV_ID and make boss id a vector
		Mechanic().setName("has green").setIds({39268}).setNameInternal("Cosmic Meteor").setFailIfHit(false).setValidIfDown(true).setBoss(&boss_arkk),
		//Mechanic().setName("didn't block the goop").setIds({MECHANIC_ARKK_GOOP}).setBoss(&boss_arkk).setCanEvade(false),

		//Sorrowful Spellcaster
		Mechanic("stood in red circle", {61463}, &boss_ai, true, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Elemental Whirl", ""),
		//Wind
		Mechanic("was hit by a windsphere", {61487,61565}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Fulgor Sphere", ""),
		Mechanic("was hit by wind blades", {61574}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Elemental Manipulation", ""),
		//Mechanic("was launched in the air", {61205}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Wind Burst", ""),
		//Mechanic("stood in wind", {61470}, & boss_ai, false, false, verbosity_chart, false, true, target_location_dst, 5000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Volatile Wind", ""),
		//Mechanic("was hit by lightning", {61190}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Call of Storms", ""),
		//Fire
		Mechanic("was hit by a fireball", {61273,61582}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Roiling Flames", ""),
		//TODO: Mechanic("was hit by fire blades", {}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Elemental Manipulation", ""),
		//Mechanic("stood in fire", {61548}, & boss_ai, false, false, verbosity_all, false, true, target_location_dst, 5000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Volatile Fire", ""),
		Mechanic("was hit by a meteor", {61348,61439}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Call Meteor", ""),
		Mechanic("was hit by firestorm", {61445}, &boss_ai, true, false, verbosity_all, false, false, target_location_dst, 1000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Firestorm", ""),
		//Water
		Mechanic("was hit by a whirlpool", {61349,61177}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Torrential Bolt", ""),
		//TODO: Mechanic("was hit by water blades", {}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Elemental Manipulation", ""),
		//Mechanic("stood in water", {61419}, & boss_ai, false, false, verbosity_all, false, true, target_location_dst, 5000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Volatile Waters", ""),
		//Dark
		Mechanic("was hit by a laser", {61344,61499}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Focused Wrath", ""),
		//TODO: Mechanic("was hit by a laser blade", {}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Empathic Manipulation", ""),
		//TODO: Mechanic("was stunned by fear", {}, &boss_ai, false, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "", ""),

		//Icebrood Construct
		Mechanic("was hit by shockwave", {57948,57472,57779}, &boss_icebrood_construct, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Shock Wave", ""),
		Mechanic("was hit by arm swing", {57516}, &boss_icebrood_construct, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Arm Swing", ""),
		Mechanic("was hit by ice orb", {57690}, &boss_icebrood_construct, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Shatter", ""),
		Mechanic("was hit by ice crystal", {57663}, &boss_icebrood_construct, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Crystal", ""),
		Mechanic("was hit by flail", {57678,57463}, &boss_icebrood_construct, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Flail", ""),
		Mechanic("was hit by deadly shockwave", {57832}, &boss_icebrood_construct, true, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Deadly Ice Shock Wave", ""),
		Mechanic("was hit by arm shatter", {57729}, &boss_icebrood_construct, true, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Shatter Arm", ""),
		
		//Voice and Claw
		Mechanic("was slammed", {58150}, &boss_voice_and_claw, false, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Hammer Slam", ""),
		Mechanic("was knocked back", {58132}, &boss_voice_and_claw, false, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Hammer Spin", ""),
		//not working Mechanic("was trapped", {727}, &boss_voice_and_claw, false, false, verbosity_all, false, true, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ghostly Grasp", ""), 

		//Fraenir of Jormag
		Mechanic("was hit by quake", {58811}, &boss_fraenir, false, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Icequake", ""),
		Mechanic("was launched by missle", {58520}, &boss_fraenir, true, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Frozen Missle", ""),
		Mechanic("was frozen", {58376}, &boss_fraenir, true, false, verbosity_all, false, true, target_location_dst, 10000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Torrent of Ice", ""),
		Mechanic("was hit by icewave", {58740}, &boss_fraenir, false, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Shock Wave", ""),
		Mechanic("was hit by shockwave", {58518}, &boss_fraenir, false, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Seismic Crush", ""),
		Mechanic("was hit by flail", {58647,58501}, &boss_fraenir, false, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Flail", ""),
		Mechanic("was hit by arm shatter", {58515}, &boss_fraenir, false, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Shatter Arm", ""),

		//Boneskinner
		Mechanic("stood in grasp", {58233}, &boss_boneskinner, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Grasp", ""),
		Mechanic("was hit by charge", {58851}, &boss_boneskinner, true, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Charge", ""),
		Mechanic("was launched by wind", {58546}, &boss_boneskinner, true, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Death Wind", ""),

		//Whisper of Jormag
		Mechanic("was hit by a slice", {59076}, &boss_whisper, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Icy Slice", ""),
		Mechanic("was hit by a tornado", {59255}, &boss_whisper, false, false, verbosity_chart, false, true, target_location_dst, 5000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Ice Tempest", ""),
		Mechanic("has chains", {59120}, &boss_whisper, false, true, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Chains of Frost", ""),
		Mechanic("was hit by chains", {59159}, &boss_whisper, true, true, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Chains of Frost", ""),
		Mechanic("was hit by own aoe", {59102}, & boss_whisper, true, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Spreading Ice", ""),
		Mechanic("was hit by other's aoe", {59468}, &boss_whisper, true, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Spreading Ice", ""),
		Mechanic("soaked damage", {59441}, &boss_whisper, false, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Lethal Coalescence", ""),

		//Cold War
		//too many procs Mechanic("was hit by icy echoes", {60354}, &boss_coldwar, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Icy Echoes", ""), 
		//not working Mechanic("was frozen", {60371}, &boss_coldwar, true, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Flash Freeze", ""), 
		Mechanic("was hit by assassins", {60308}, &boss_coldwar, true, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Call Assassins", ""),
		Mechanic("stood in flames", {60171}, &boss_coldwar, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Flame Wall", ""),
		Mechanic("was run over", {60132}, &boss_coldwar, true, false, verbosity_all, true, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Charge!", ""),
		Mechanic("was hit by detonation", {60006}, & boss_coldwar, true, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Detonate", ""),
		Mechanic("soaked damage", {60545}, &boss_coldwar, false, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Lethal Coalescence", ""),

		//Mai Trin
		Mechanic().setName("hit by cone attack").setIds({65749}).setBoss(&boss_mai_trin),
		Mechanic().setName("hit by side cone attack").setIds({66089}).setBoss(&boss_mai_trin),
		Mechanic().setName("hit by shockwave attack").setIds({65031, 67866}).setBoss(&boss_mai_trin),
		Mechanic().setName("stood in puddle").setIds({64044, 67832}).setBoss(&boss_mai_trin),
		Mechanic().setName("hit by yellow circle").setIds({66568}).setBoss(&boss_mai_trin),
		Mechanic().setName("received Exposed stack").setIds({64936}).setSpecialRequirement(requirementSpecificBoss).setBoss(&boss_mai_trin),
		Mechanic().setName("was selected for green").setFailIfHit(false).setIds({65900, 67831}).setBoss(&boss_mai_trin),
		Mechanic().setName("received green debuff").setFailIfHit(false).setIds({67872}).setBoss(&boss_mai_trin),
		Mechanic().setName("downed by green").setIds({67954}).setBoss(&boss_mai_trin),
		Mechanic().setName("selected for bomb").setFailIfHit(false).setIds({67856}).setBoss(&boss_mai_trin),

		//Ankka
		Mechanic().setName("hit by hands AoE").setIds({66246}).setBoss(&boss_ankka),
		Mechanic().setName("hit by pull AoE").setIds({67160}).setBoss(&boss_ankka),
		Mechanic().setName("hit by in between sections AoE").setIds({66728}).setBoss(&boss_ankka),
		Mechanic().setName("hit by Kraits").setIds({66824}).setBoss(&boss_ankka),
		Mechanic().setName("hit by Quaggan Explosion").setIds({64669}).setBoss(&boss_ankka),

		//Minister Li
		Mechanic().setName("hit by wave").setIds({64952, 67825}).setBoss(&boss_minister_li),
		Mechanic().setName("hit by burst").setIds({66465, 65378}).setBoss(&boss_minister_li),
		Mechanic().setName("hit by rush").setIds({66090, 64619, 67824, 67943}).setBoss(&boss_minister_li),
		Mechanic().setName("got bomb").setIds({64277, 67982}).setBoss(&boss_minister_li),
		Mechanic().setName("was selected for green").setIds({64300, 68004}).setBoss(&boss_minister_li),
		Mechanic().setName("stood in flames").setIds({65608, 68028}).setBoss(&boss_minister_li),
		Mechanic().setName("overlapped red circles").setIds({65243}).setBoss(&boss_minister_li),
		Mechanic().setName("was fixated").setIds({66140}).setSpecialRequirement(requirementSpecificBoss).setBoss(&boss_minister_li),
		Mechanic().setName("hit by bladestorm").setIds({63838, 63550, 65569}).setBoss(&boss_minister_li),
		Mechanic().setName("hit by big laser").setIds({64016}).setBoss(&boss_minister_li),
		Mechanic().setName("received Debilitated stack").setIds({67972}).setSpecialRequirement(requirementSpecificBoss).setBoss(&boss_minister_li),
		Mechanic().setName("received Infirmity stack").setIds({67965}).setSpecialRequirement(requirementSpecificBoss).setBoss(&boss_minister_li),
		Mechanic().setName("received Exposed stack").setIds({64936}).setSpecialRequirement(requirementSpecificBoss).setBoss(&boss_minister_li),


		//Harvest Temple
		Mechanic().setName("received Void debuff").setIds({64524}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Void").setIds({66566}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Jormag breath").setIds({65517, 66216, 67607}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Primordus Slam").setIds({64527}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Crystal Barrage").setIds({66790}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Kralkatorrik Beam").setIds({65017}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Modremoth Shockwave").setIds({64810}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Zhaitan Scream").setIds({66658}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Whirlpool").setIds({65252}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Soo-Won Tsunami").setIds({64748, 66489}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("hit by Soo-Won Claw").setIds({63588}).setBoss(&boss_the_dragonvoid),
		Mechanic().setName("was revealed").setFailIfHit(false).setIds({890}).setSpecialRequirement(requirementOnSelfRevealedInHarvestTemple).setBoss(&boss_the_dragonvoid),
	};
	return *mechanics;
}
