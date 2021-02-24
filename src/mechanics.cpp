#include "mechanics.h"

bool has_logged_mechanic = false;

Mechanic::Mechanic(Boss* new_boss = &boss_generic, //TODO: should this be explicit?
	std::string new_name = "",
	std::initializer_list<uint32_t> new_ids = {},
	std::string new_description = "",
	bool new_fail_if_hit = true,
	bool new_is_interupt = false,
	bool new_is_multihit = true,
	TargetLocation new_target_is_dst = TargetLocation::Destination,
	uint64_t new_frequency_player = 2000,
	uint64_t new_frequency_global = 0,
	uint8_t new_is_activation = ACTV_NONE,
	uint8_t new_is_buffremove = CBTB_NONE,
	bool new_can_evade = true,
	bool new_can_block = true,
	bool new_can_invuln = true,
	bool(*new_special_requirement)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player) = requirementDefault,
	int64_t(*new_special_value)(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player) = valueDefault,
	std::string new_name_internal = "",
	bool new_valid_if_down = false,
	int32_t new_overstack_value = -1,
	int32_t new_value = -1,
	Verbosity new_verbosity = Verbosity::All)
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
	target_is_dst = new_target_is_dst;

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

	if (verbosity == Verbosity::None) return false;

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

	if (target_is_dst == TargetLocation::Destination)
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

bool requirementDefault(const ::Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
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

int64_t valueDefault(const Mechanic &current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player* current_player)
{
	return 1;
}

int64_t valueDhuumShackles(const Mechanic & current_mechanic, cbtevent* ev, ag* ag_src, ag* ag_dst, Player * player_src, Player * player_dst, Player * current_player)
{
	return (int64_t)30 - ((int64_t)ev->value / 1000);
}

std::vector<Mechanic>& getMechanics()
{
	static std::vector<Mechanic>* mechanics = new std::vector<Mechanic>
		{
			Mechanic(&boss_vg,"was teleported",{31392,31860},"Unstable Magic Spike"),
			//I'm not sure why this mechanic has 4 ids, but it appears to for some reason
			//all these ids are for when 4 people are in the green circle
			//it appears to be a separate id for the 90% hp blast when <4 people are in the green
			//all 4 ids are called "Distributed Magic"
			Mechanic(&boss_vg, "stood in the green circle",{31340,31391,31529,31750},"Distributed Magic",false,false,false)
				.setVerbosity(Verbosity::Chart).setCanBlock(false).setCanEvade(false).setCanInvuln(false).setFrequencyPlayer(0),

			Mechanic(&boss_gors, "was slammed", {MECHANIC_GORS_SLAM}).setIsInterupt(true),
			Mechanic(&boss_gors, "was egged", {MECHANIC_GORS_EGG}),
			Mechanic(&boss_gors, "touched an orb", {MECHANIC_GORS_ORB}).setSpecialRequirement(requirementBuffApply),

			Mechanic(&boss_sab, "got a sapper bomb", {MECHANIC_SAB_SAPPER_BOMB}).setFailIfHit(false).setValidIfDown(true),
			Mechanic(&boss_sab, "got a time bomb", {MECHANIC_SAB_TIME_BOMB}).setFailIfHit(false).setValidIfDown(true),
			Mechanic(&boss_sab, "stood in cannon fire", {MECHANIC_SAB_CANNON}),
			//Mechanic().setName("touched the flame wall").setIds({MECHANIC_SAB_FLAMEWALL}).setBoss(&boss_sab),

			Mechanic(&boss_sloth, "was hit with tantrum", {MECHANIC_SLOTH_TANTRUM}),
			Mechanic(&boss_sloth, "got a bomb", {MECHANIC_SLOTH_BOMB}).setFailIfHit(false).setFrequencyPlayer(6000),
			Mechanic(&boss_sloth, "stood in bomb aoe", {MECHANIC_SLOTH_BOMB_AOE}).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_sloth, "was hit by flame breath", {MECHANIC_SLOTH_FLAME_BREATH}),
			Mechanic(&boss_sloth, "was hit by shake", {MECHANIC_SLOTH_SHAKE}),
			Mechanic(&boss_sloth, "is fixated", {MECHANIC_SLOTH_FIXATE}).setFailIfHit(false),

			Mechanic(&boss_trio,"threw a beehive",{34533}, "Beehive").setFailIfHit(false).setIsMultihit(false).setTargetIsDst(TargetLocation::Source).setFrequencyPlayer(0).setIsActivation(ACTV_NORMAL).setCanBlock(false).setCanEvade(false).setCanInvuln(false),
			Mechanic(&boss_trio,"threw an oil keg",{34471},"Throw").setFailIfHit(false).setIsMultihit(false).setTargetIsDst(TargetLocation::Source).setIsActivation(ACTV_NORMAL).setCanBlock(false).setCanEvade(false).setCanInvuln(false),

			//Mechanic().setName("was hadoukened").setIds({MECHANIC_MATT_HADOUKEN_HUMAN,MECHANIC_MATT_HADOUKEN_ABOM}).setBoss(&boss_matti),
			Mechanic(&boss_matti, "reflected shards", {MECHANIC_MATT_SHARD_HUMAN,MECHANIC_MATT_SHARD_ABOM}).setTargetIsDst(TargetLocation::Source),
			Mechanic(&boss_matti, "got a bomb", {MECHANIC_MATT_BOMB}).setFailIfHit(false).setFrequencyPlayer(12000),
			Mechanic(&boss_matti, "got a corruption", {MECHANIC_MATT_CORRUPTION}).setFailIfHit(false),
			Mechanic(&boss_matti, "is sacrificed", {MECHANIC_MATT_SACRIFICE}).setFailIfHit(false),
			Mechanic(&boss_matti,"touched a ghost",{34413},"Surrender"),
			//Mechanic("touched an icy patch",{26766},&boss_matti,true,false,verbosity_all,false,true,target_location_dst,2000,0,-1,10000,ACTV_NONE,CBTB_NONE,true,true,true,requirementDefault,valueDefault,
			//"Slow",""),//look for Slow application with 10 sec duration. Disabled because some mob in Istan applies the same duration of slow
			Mechanic(&boss_matti,"stood in tornado",{34466},"Fiery Vortex"),
			Mechanic(&boss_matti,"stood in storm cloud",{34543},"Thunder"),

			Mechanic(&boss_kc, "is fixated", {MECHANIC_KC_FIXATE}).setFailIfHit(false),
			//Mechanic().setName("is west fixated").setIds({MECHANIC_KC_FIXATE_WEST}).setFailIfHit(false).setBoss(&boss_kc),
			Mechanic(&boss_kc, "touched the core").setFailIfHit(false).setTargetIsDst(TargetLocation::Source).setFrequencyPlayer(8000).setSpecialRequirement(requirementKcCore),
			Mechanic(&boss_kc,"was squashed",{35086},"Tower Drop").setIsInterupt(true),
			Mechanic(&boss_kc, "stood in donut",{35137,34971,35086},"Phantasmal Blades"),

			Mechanic(&boss_xera,"stood in red half", {34921}).setFrequencyPlayer(4000).setCanEvade(false).setCanBlock(false),//TODO:check internal name
			Mechanic(&boss_xera, "has magic", {MECHANIC_XERA_MAGIC}).setFailIfHit(false).setValidIfDown(true).setValue(15000),
			Mechanic(&boss_xera, "used magic", {MECHANIC_XERA_MAGIC_BUFF}).setFailIfHit(false).setTargetIsDst(TargetLocation::Source).setFrequencyGlobal(12000).setValidIfDown(true).setSpecialRequirement(requirementOnSelf).setVerbosity(Verbosity::None),
			Mechanic(&boss_xera, "triggered an orb", {MECHANIC_XERA_ORB}),
			Mechanic(&boss_xera, "stood in an orb aoe", {MECHANIC_XERA_ORB_AOE}).setFrequencyPlayer(1000).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_xera, "was teleported", {MECHANIC_XERA_PORT}).setVerbosity(Verbosity::Chart),

			Mechanic(&boss_cairn, "was teleported", {MECHANIC_CAIRN_TELEPORT}),
			Mechanic(&boss_cairn, "was slapped", {MECHANIC_CAIRN_SWEEP}).setIsInterupt(true),
			//Mechanic().setName("reflected shards").setIds({MECHANIC_CAIRN_SHARD}).setTargetIsDst(false).setBoss(&boss_cairn),
			Mechanic(&boss_cairn, "missed a green circle", {MECHANIC_CAIRN_GREEN_A,MECHANIC_CAIRN_GREEN_B,MECHANIC_CAIRN_GREEN_C,MECHANIC_CAIRN_GREEN_D,MECHANIC_CAIRN_GREEN_E,MECHANIC_CAIRN_GREEN_F}).setIsInterupt(true),

			Mechanic(&boss_sam, "was shockwaved", {MECHANIC_SAM_SHOCKWAVE}).setIsInterupt(true),
			Mechanic(&boss_sam, "was horizontally slapped", {MECHANIC_SAM_SLAP_HORIZONTAL}).setIsInterupt(true),
			Mechanic(&boss_sam, "was vertically smacked", {MECHANIC_SAM_SLAP_VERTICAL}).setIsInterupt(true),
			Mechanic(&boss_sam, "is fixated", {MECHANIC_SAM_FIXATE_SAM}).setFailIfHit(false),
			Mechanic(&boss_sam, "has big green", {MECHANIC_SAM_GREEN_BIG}).setFailIfHit(false),
			Mechanic(&boss_sam, "has small green", {MECHANIC_SAM_GREEN_SMALL}).setFailIfHit(false),

			Mechanic(&boss_deimos,"touched an oil",{37716}, "Rapid Decay").setFrequencyPlayer(5000).setSpecialRequirement(requirementDeimosOil),
			Mechanic(&boss_deimos, "was smashed", {MECHANIC_DEIMOS_SMASH,MECHANIC_DEIMOS_SMASH_INITIAL,MECHANIC_DEIMOS_SMASH_END_A,MECHANIC_DEIMOS_SMASH_END_B}),
			Mechanic(&boss_deimos, "closed a tear", {MECHANIC_DEIMOS_TEAR}).setFailIfHit(false),
			Mechanic(&boss_deimos,"has the teleport",{37730},"Chosen by Eye of Janthir").setFailIfHit(false).setValidIfDown(true).setCanBlock(false).setCanEvade(false).setCanInvuln(false).setFrequencyPlayer(0),
			Mechanic(&boss_deimos,"was teleported",{38169}).setFrequencyPlayer(0).setCanBlock(false).setCanEvade(false).setCanInvuln(false).setVerbosity(Verbosity::Chart).setFailIfHit(false).setValidIfDown(true).setIsMultihit(false),

			Mechanic(&boss_sh, "stood in inner ring", {MECHANIC_HORROR_DONUT_INNER}).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_sh, "stood in outer ring", {MECHANIC_HORROR_DONUT_OUTER}).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_sh, "stood in torment aoe", {MECHANIC_HORROR_GOLEM_AOE}),
			Mechanic(&boss_sh, "stood in pie slice", {MECHANIC_HORROR_PIE_4_A,MECHANIC_HORROR_PIE_4_B}).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_sh, "touched a scythe", {MECHANIC_HORROR_SCYTHE}),
			Mechanic(&boss_sh, "took fixate", {MECHANIC_HORROR_FIXATE}).setFailIfHit(false).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_sh, "was debuffed", {MECHANIC_HORROR_DEBUFF}).setFailIfHit(false).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_sh, "CCed a tormented dead",{872,833,31465}, "Stun, Daze, Temporal stasis").setValidIfDown(true).setTargetIsDst(TargetLocation::Source).setSpecialRequirement(requirementShTdCc),

			Mechanic(&boss_soul_eater, "was puked on", {MECHANIC_EATER_PUKE}).setFrequencyPlayer(3000).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_soul_eater, "stood in web", {MECHANIC_EATER_WEB}).setFrequencyPlayer(3000).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_soul_eater, "got an orb", {MECHANIC_EATER_ORB}).setFrequencyPlayer(ms_per_tick).setFailIfHit(false),
			Mechanic(&boss_soul_eater, "threw an orb", { 47942 }, "Reclaimed Energy").setTargetIsDst(TargetLocation::Source).setIsActivation(ACTV_NORMAL).setFailIfHit(false),

			Mechanic(&boss_ice_king, "got a green",{47013}, "Hailstorm").setFrequencyPlayer(0).setFailIfHit(false).setValidIfDown(true).setIsMultihit(false).setVerbosity(Verbosity::Chart),

			Mechanic(&boss_cave, "CCed an eye",{872}, "Stun").setTargetIsDst(TargetLocation::Source).setFrequencyPlayer(0).setSpecialRequirement(requirementCaveEyeCc).setFailIfHit(false).setValidIfDown(true).setIsMultihit(false),

			Mechanic(&boss_dhuum, "touched a messenger", {MECHANIC_DHUUM_GOLEM}),
			Mechanic(&boss_dhuum, "is shackled", {MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false).setTargetIsDst(TargetLocation::Source),
			Mechanic(&boss_dhuum, "is shackled", {MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false),
			//Mechanic().setName("popped shackles").setIds({MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false).setIsBuffremove(CBTB_MANUAL).setTargetIsDst(false).setSpecialValue(valueDhuumShackles).setBoss(&boss_dhuum),
			//Mechanic().setName("popped shackles").setIds({MECHANIC_DHUUM_SHACKLE}).setFailIfHit(false).setIsBuffremove(CBTB_MANUAL).setSpecialValue(valueDhuumShackles).setBoss(&boss_dhuum),
			Mechanic(&boss_dhuum, "has affliction", {MECHANIC_DHUUM_AFFLICTION}).setFrequencyPlayer(13000 + ms_per_tick).setFailIfHit(false).setValidIfDown(true),
			Mechanic(&boss_dhuum, "took affliction damage",{48121}, "Arcing Affliction").setFrequencyPlayer(0).setCanBlock(false).setCanEvade(false).setCanInvuln(false).setVerbosity(Verbosity::Chart).setFailIfHit(false).setValidIfDown(true).setIsMultihit(false),
			Mechanic(&boss_dhuum, "stood in a crack", { MECHANIC_DHUUM_CRACK }),
			Mechanic(&boss_dhuum, "stood in a poison mark", {MECHANIC_DHUUM_MARK}).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_dhuum, "was sucked center", {MECHANIC_DHUUM_SUCK_AOE}),
			Mechanic(&boss_dhuum, "stood in dip aoe", {MECHANIC_DHUUM_TELEPORT_AOE}),
			//Mechanic().setName("died on green").setIds({MECHANIC_DHUUM_GREEN_TIMER}).setIsBuffremove(CBTB_MANUAL).setOverstackValue(0).setBoss(&boss_dhuum),
			//Mechanic().setName("aggroed a messenger").setNameInternal("").setTargetIsDst(false).setFailIfHit(false).setFrequencyPlayer(0).setValidIfDown(true).setBoss(&boss_dhuum).setSpecialRequirement(requirementDhuumMessenger),
			Mechanic(&boss_dhuum, "was snatched", {MECHANIC_DHUUM_SNATCH}).setSpecialRequirement(requirementDhuumSnatch),
			//Mechanic().setName("canceled button channel").setIds({MECHANIC_DHUUM_BUTTON_CHANNEL}).setIsActivation(ACTV_CANCEL_CANCEL).setBoss(&boss_dhuum),
			Mechanic(&boss_dhuum, "stood in cone", {MECHANIC_DHUUM_CONE}),

			Mechanic(&boss_ca, "was squashed", {MECHANIC_AMAL_SQUASH}).setIsInterupt(true),
			Mechanic(&boss_ca, "used a sword", { 52325 }, "Conjured Greatsword").setTargetIsDst(TargetLocation::Source).setIsActivation(ACTV_NORMAL).setFailIfHit(false),
			Mechanic(&boss_ca, "used a shield", { 52780 }, "Conjured Protection").setTargetIsDst(TargetLocation::Source).setIsActivation(ACTV_NORMAL).setFailIfHit(false),

			Mechanic(&boss_largos, "was shockwaved", {MECHANIC_LARGOS_SHOCKWAVE}).setIsInterupt(true),
			Mechanic(&boss_largos, "was waterlogged", {MECHANIC_LARGOS_WATERLOGGED}).setVerbosity(Verbosity::Chart).setValidIfDown(true).setFrequencyPlayer(1),
			Mechanic(&boss_largos, "was bubbled", {MECHANIC_LARGOS_BUBBLE}),
			Mechanic(&boss_largos, "has a tidal pool", {MECHANIC_LARGOS_TIDAL_POOL}).setFailIfHit(false),
			Mechanic(&boss_largos, "stood in geyser", {MECHANIC_LARGOS_GEYSER}),
			Mechanic(&boss_largos, "was dashed over", {MECHANIC_LARGOS_DASH}),
			Mechanic(&boss_largos, "had boons stolen", {MECHANIC_LARGOS_BOON_RIP}),
			Mechanic(&boss_largos, "stood in whirlpool", {MECHANIC_LARGOS_WHIRLPOOL}),

			Mechanic(&boss_qadim, "was shockwaved", {MECHANIC_QADIM_SHOCKWAVE_A,MECHANIC_QADIM_SHOCKWAVE_B}),
			Mechanic(&boss_qadim, "stood in arcing fire", {MECHANIC_QADIM_ARCING_FIRE_A,MECHANIC_QADIM_ARCING_FIRE_B,MECHANIC_QADIM_ARCING_FIRE_C}).setVerbosity(Verbosity::Chart),
			//Mechanic().setName("stood in giant fireball").setIds({MECHANIC_QADIM_BOUNCING_FIREBALL_BIG_A,MECHANIC_QADIM_BOUNCING_FIREBALL_BIG_B,MECHANIC_QADIM_BOUNCING_FIREBALL_BIG_C}).setBoss(&boss_qadim),
			Mechanic(&boss_qadim, "was teleported", {MECHANIC_QADIM_TELEPORT}).setValidIfDown(true),
			Mechanic(&boss_qadim, "stood in hitbox", { 52461 }, "Sea of Flame"),

			Mechanic(&boss_adina,"was blinded",{56593}, "Radiant Blindness").setFailIfHit(false).setVerbosity(Verbosity::Chart),
			Mechanic(&boss_adina, "looked at eye", {56114}, "Diamond Palisade").setFailIfHit(false).setCanBlock(false),
			Mechanic(&boss_adina, "touched pillar ripple",{56558},"Tectonic Upheaval"),
			Mechanic(&boss_adina, "touched a mine",{56141}, "Stalagmites").setFrequencyPlayer(1000),
			//Mechanic("has pillar",{47860},&boss_adina,false,false,verbosity_all,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,false,false,false,
			//	requirementDefault,valueDefault,"",""),//wrong id?
				//TODO: get adina pillar id(s?)

			Mechanic(&boss_sabir, "touched big tornado",{56202}, "Dire Drafts"),
			Mechanic(&boss_sabir, "was shockwaved",{56643},"Unbridled Tempest").setCanEvade(false).setCanBlock(false),
			Mechanic(&boss_sabir, "wasn't in bubble",{56372},"Fury of the Storm").setCanBlock(false).setCanEvade(false).setCanInvuln(false),
			Mechanic(&boss_sabir, "was bopped at phase", {56094}, "Walloping Wind").setFrequencyPlayer(0).setVerbosity(Verbosity::Chart).setIsMultihit(false),

			Mechanic(&boss_qadim2, "is tank",{56510}, "Fixated").setFailIfHit(false).setValidIfDown(true).setIsMultihit(false).setFrequencyPlayer(0).setCanBlock(false).setCanEvade(false).setCanInvuln(false),
			Mechanic(&boss_qadim2, "touched lava", {56180,56378,56541}, "Residual Impact, Pylon Debris Field"),//ids are big,small(CM),pylon
			Mechanic(&boss_qadim2, "was struck by small lightning",{56656}, "Brandstorm Lightning").setVerbosity(Verbosity::Chart).setIsMultihit(false).setFrequencyPlayer(1000),
			Mechanic(&boss_qadim2, "was hit by triple lightning",{56527}, "Rain of Chaos").setIsMultihit(false).setFrequencyPlayer(1000),
			Mechanic(&boss_qadim2, "touched arcing line",{56145}, "Chaos Called").setIsMultihit(false),
			Mechanic(&boss_qadim2, "was shockwaved",{56134}, "Force of Retaliation").setIsInterupt(true),
			Mechanic(&boss_qadim2, "touched purple rectangle",{56441}, "Force of Havoc").setVerbosity(Verbosity::Chart),
			Mechanic(&boss_qadim2, "was ran over",{56616}, "Battering Blitz"),
			Mechanic(&boss_qadim2, "was sniped",{56332}, "Caustic Chaos").setValidIfDown(true).setIsMultihit(false).setFrequencyPlayer(100),
			Mechanic(&boss_qadim2, "was splashed by sniper",{56543}, "Caustic Chaos").setVerbosity(Verbosity::Chart).setFailIfHit(false).setValidIfDown(true).setIsMultihit(false).setFrequencyPlayer(100),
			//Mechanic("has lightning", {51371},&boss_qadim2,false,true,verbosity_all,false,false,target_location_dst,0,0,-1,-1,ACTV_NONE,CBTB_NONE,true,true,true,
			//	requirementDefault,valueDefault,"",""),


			Mechanic(&boss_fotm_generic, "got a flux bomb", {MECHANIC_FOTM_FLUX_BOMB}).setFailIfHit(false),
			//Mechanic().setName("vomited on someone").setIds({MECHANIC_NIGHTMARE_VOMIT}).setTargetIsDst(false).setBoss(&boss_fotm_generic),

			Mechanic(&boss_mama, "was hit by whirl", {MECHANIC_MAMA_WHIRL,MECHANIC_MAMA_WHIRL_NORMAL}),
			Mechanic(&boss_mama, "was knocked", {MECHANIC_MAMA_KNOCK}),
			Mechanic(&boss_mama, "was leaped on", {MECHANIC_MAMA_LEAP}),
			Mechanic(&boss_mama, "stood in acid", {MECHANIC_MAMA_ACID}),
			Mechanic(&boss_mama, "was smashed by a knight", {MECHANIC_MAMA_KNIGHT_SMASH}),

			Mechanic(&boss_siax, "stood in acid", {MECHANIC_SIAX_ACID}),

			Mechanic(&boss_ensolyss, "was ran over", {MECHANIC_ENSOLYSS_LUNGE}),
			Mechanic(&boss_ensolyss, "was smashed", {MECHANIC_ENSOLYSS_SMASH}),

			Mechanic(&boss_arkk, "stood in a pie slice", {MECHANIC_ARKK_PIE_A,MECHANIC_ARKK_PIE_B,MECHANIC_ARKK_PIE_C}),
			//Mechanic().setName("was feared").setIds({MECHANIC_ARKK_FEAR}),
			Mechanic(&boss_arkk, "was smashed", {MECHANIC_ARKK_OVERHEAD_SMASH}),
			Mechanic(&boss_arkk, "has a bomb", {MECHANIC_ARKK_BOMB}).setFailIfHit(false).setValidIfDown(true), //TODO Add BOSS_ARTSARIIV_ID and make boss id a vector
			Mechanic(&boss_arkk, "has green", { 39268 }, "Cosmic Meteor").setFailIfHit(false).setValidIfDown(true),
			//Mechanic().setName("didn't block the goop").setIds({MECHANIC_ARKK_GOOP}).setBoss(&boss_arkk).setCanEvade(false),


			//Sorrowful Spellcaster
			Mechanic(&boss_ai, "stood in red circle", {61463}, "Elemental Whirl").setIsMultihit(false),
			//Wind
			Mechanic(&boss_ai, "was hit by a windsphere", {61487,61565},"Fulgor Sphere"),
			Mechanic(&boss_ai, "was hit by wind blades", {61574}, "Elemental Manipulation"),
			//Mechanic("was launched in the air", {61205}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Wind Burst", ""),
			//Mechanic("stood in wind", {61470}, & boss_ai, false, false, verbosity_chart, false, true, target_location_dst, 5000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Volatile Wind", ""),
			//Mechanic("was hit by lightning", {61190}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Call of Storms", ""),
			//Fire
			Mechanic(&boss_ai, "was hit by a fireball", {61273,61582}, "Roiling Flames"),
			//TODO: Mechanic("was hit by fire blades", {}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Elemental Manipulation", ""),
			//Mechanic("stood in fire", {61548}, & boss_ai, false, false, verbosity_all, false, true, target_location_dst, 5000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Volatile Fire", ""),
			Mechanic(&boss_ai, "was hit by a meteor", {61348,61439}, "Call Meteor"),
			Mechanic(&boss_ai, "was hit by firestorm", {61445}, "Firestorm"),
			//Water
			Mechanic(&boss_ai, "was hit by a whirlpool", {61349,61177}, "Torrential Bolt"),
			//TODO: Mechanic("was hit by water blades", {}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Elemental Manipulation", ""),
			//Mechanic("stood in water", {61419}, & boss_ai, false, false, verbosity_all, false, true, target_location_dst, 5000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Volatile Waters", ""),
			//Dark
			Mechanic(&boss_ai, "was hit by a laser", {61344,61499}, "Focused Wrath"),
			//TODO: Mechanic("was hit by a laser blade", {}, &boss_ai, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Empathic Manipulation", ""),
			//TODO: Mechanic("was stunned by fear", {}, &boss_ai, false, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "", ""),
		


			Mechanic(&boss_boneskinner, "was hit by charge", { MECHANIC_BONESKINNER_CHARGE }).setIsInterupt(true),
			Mechanic(&boss_boneskinner, "was hit by Death Wind", { MECHANIC_BONESKINNER_DEATH_WIND }).setIsInterupt(true),

				//this should work, but computer says no
			//Mechanic(&boss_kodan, "was trapped", { MECHANIC_KODAN_TRAP }).setSpecialRequirement(requirementBuffApply),

			Mechanic(&boss_fraenir, "was hit by icequake", { MECHANIC_FRAENIR_ICEQUAKE }),
			Mechanic(&boss_fraenir, "was hit by shock wave", { MECHANIC_FRAENIR_ICE_SHOCK_WAVE }),
			Mechanic(&boss_fraenir, "was frozen", { MECHANIC_FRAENIR_FROZEN }).setSpecialRequirement(requirementBuffApply),

			Mechanic(&boss_whisper, "was hit by chains", { MECHANIC_WHISPER_CHAINS }),
			Mechanic(&boss_whisper, "was hit by own spreading ice", { MECHANIC_WHISPER_OWN_ICE }),
			Mechanic(&boss_whisper, "was hit by other spreading ice", { MECHANIC_WHISPER_OTHER_ICE }),
			Mechanic(&boss_whisper, "was hit by icy slice", { MECHANIC_WHISPER_ICY_SLICE }),
			Mechanic(&boss_whisper, "was hit by ice tornado", { MECHANIC_WHISPER_ICE_TEMPEST }),
			Mechanic(&boss_whisper, "has a chain", { MECHANIC_WHISPER_HAS_CHAINS }).setFailIfHit(false).setSpecialRequirement(requirementBuffApply),

				//Icebrood Construct
			Mechanic(&boss_icebrood_construct, "was hit by deadly shock wave", { MECHANIC_ICEBROOD_SHOCK_WAVE_DEADLY }),
			Mechanic(&boss_icebrood_construct, "was hit by arm swing", { MECHANIC_ICEBROOD_ARM_SWING }),
			Mechanic(&boss_icebrood_construct, "was hit by shock wave", { MECHANIC_ICEBROOD_SHOCK_WAVE_1, MECHANIC_ICEBROOD_SHOCK_WAVE_2, MECHANIC_ICEBROOD_SHOCK_WAVE_3 }),
			Mechanic(&boss_icebrood_construct, "was hit by ice shatter", { MECHANIC_ICEBROOD_SHATTER }),
			Mechanic(&boss_icebrood_construct, "was hit by crystal", { MECHANIC_ICEBROOD_CRYSTAL }),
			Mechanic(&boss_icebrood_construct, "was hit by flail", { MECHANIC_ICEBROOD_FLAIL_1, MECHANIC_ICEBROOD_FLAIL_2 }),

			//Cold War
			//too many procs Mechanic("was hit by icy echoes", {60354}, &boss_coldwar, false, false, verbosity_chart, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Icy Echoes", ""), 
			//not working Mechanic("was frozen", {60371}, &boss_coldwar, true, false, verbosity_all, false, false, target_location_dst, 2000, 0, -1, -1, ACTV_NONE, CBTB_NONE, true, true, true, requirementDefault, valueDefault, "Flash Freeze", ""), 
			Mechanic(&boss_coldwar, "was hit by assassins", { 60308 }, "Call Assassins"),
			Mechanic(&boss_coldwar, "stood in flames", {60171}, "Flame Wall"),
			Mechanic(&boss_coldwar, "was run over", {60132}, "Charge!"),
			Mechanic(&boss_coldwar, "was hit by detonation", {60006}, "Detonate"),
			Mechanic(&boss_coldwar, "soaked damage", {60545}, "Lethal Coalescence").setFailIfHit(false),
	};
	return *mechanics;
}
