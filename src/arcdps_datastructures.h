#pragma once

#include <stdint.h>

/* arcdps export table */
typedef struct arcdps_exports {
	uintptr_t size; /* size of exports table */
	uint32_t sig; /* pick a number between 0 and uint32_t max that isn't used by other modules */
	uint32_t imguivers; /* set this to IMGUI_VERSION_NUM. if you don't use imgui, 18000 (as of 2021-02-02) */
	const char* out_name; /* name string */
	const char* out_build; /* build string */
	void* wnd_nofilter; /* wndproc callback, fn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) */
	void* combat; /* combat event callback, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
	void* imgui; /* id3dd9::present callback, before imgui::render, fn(uint32_t not_charsel_or_loading) */
	void* options_end; /* id3dd9::present callback, appending to the end of options window in arcdps, fn() */
	void* combat_local;  /* combat event callback like area but from chat log, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
	void* wnd_filter; /* wndproc callback like above, input filered using modifiers */
	void* options_windows; /* called once per 'window' option checkbox, with null at the end, non-zero return disables drawing that option, fn(char* windowname) */
} arcdps_exports;

/* iff */
enum iff {
	IFF_FRIEND, // green vs green, red vs red
	IFF_FOE, // green vs red
	IFF_UNKNOWN // something very wrong happened
};

/* combat result (physical) */
enum cbtresult {
	CBTR_STRIKE_DAMAGENORMAL, // damage is strike
	CBTR_STRIKE_DAMAGECRIT, // strike was crit
	CBTR_STRIKE_DAMAGEGLANCE, // strike was glance
	CBTR_BLOCK, // blocked eg. mesmer shield 4
	CBTR_EVADE, // evaded, eg. dodge or mesmer sword 2
	CBTR_INTERRUPT, // action was interrupted
	CBTR_ABSORB, // invulnerable or absorbed eg. guardian elite
	CBTR_BLIND, // action missed
	CBTR_KILLINGBLOW, // target was killed by skill
	CBTR_DOWNED, // target was downed by skill
	CBTR_DEFIANCE_DAMAGENORMAL, // damage is to defiance
	CBTR_SKILLCAST, // on-skill-use signal event
	CBTR_CROWDCONTROL, // target was crowdcontrolled
	CBTR_INVERT, // damage was inverted
	CBTR_BUFF_DAMAGECYCLE, // buff damage happened on tick timer
	CBTR_BUFF_DAMAGENOTCYCLE, // buff damage happened outside tick timer
	CBTR_BUFF_DAMAGENOTCYCLEDMGTOTARGETONHIT, // buff damage happened to target on hitting target
	CBTR_BUFF_DAMAGENOTCYCLEDMGTOSOURCEONHIT, // buff damage happened to source on hitting target
	CBTR_BUFF_DAMAGENOTCYCLEDMGTOTARGETONSTACKREMOVE, // buff damage happened to target on buff removal
	CBTR_UNKNOWN
};

enum cbtanimation {
	ACTV_NONE,
	ACTV_START_DEFUNC,
	ACTV_QUICKNESS_DEFUNC,
	ACTV_MINIMUM, // stopped animation with reaching minimum of first trigger point or tooltip time
	ACTV_CANCEL, // stopped animation without reaching minimum of first trigger point or tooltip time
	ACTV_RESET, // animation completed fully
	ACTV_NODATA, // same as ACTV_MINIMUM but on 0/uncertain expected duration
	ACTV_UNKNOWN
};

/* combat state change */
enum cbtstatechange {
	CBTS_COMBAT = 0, // combat events
	// src_agent: source agent
	// dst_agent: target agent
	// value: combined shield+health strike damage
	// buff_dmg: combined shield+health buff damage
	// overstack_value: shield damage
	// skillid: damage skill id
	// iff: is friend foe of enum iff
	// is_buff: skill is a buff
	// result: combat result of enum cbtresult
	// is_ninety: src is above 90% health
	// is_fifty: dst is below 50% health
	// is_moving: bit0 set if src is moving, bit1 set if dst is moving
	// is_flanking: src is flanking dst
	// is_offcycle: dst was downed at time of event

	CBTS_ENTERCOMBAT, // agent entered combat
	// src_agent: relates to agent
	// dst_agent: subgroup
	// value: prof id
	// buff_dmg: elite spec id
	// evtc: limited to squad outside instances
	// realtime: limited to squad

	CBTS_EXITCOMBAT, // agent left combat
	// src_agent: relates to agent
	// dst_agent: subgroup
	// value: prof id
	// buff_dmg: elite spec id
	// evtc: limited to squad outside instances
	// realtime: limited to squad

	CBTS_CHANGEUP, // agent is alive at time of event
	// src_agent: relates to agent
	// evtc: limited to agent table outside instances
	// realtime: limited to squad

	CBTS_CHANGEDEAD, // agent is dead at time of event
	// src_agent: relates to agent
	// evtc: limited to agent table outside instances
	// realtime: limited to squad

	CBTS_CHANGEDOWN, // agent is down at time of event
	// src_agent: relates to agent
	// evtc: limited to agent table outside instances
	// realtime: limited to squad

	CBTS_SPAWN, // agent entered tracking
	// src_agent: relates to agent
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_DESPAWN, // agent left tracking
	// src_agent: relates to agent
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_HEALTHPCTUPDATE, // agent health percentage changed
	// src_agent: relates to agent
	// dst_agent: percent * 10000 eg. 99.5% will be 9950
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_SQCOMBATSTART, // squad combat start, first player enter combat. previously named log start
	// dst_agent: 2 if map log, 3 if boss log
	// value: as uint32_t, server unix timestamp
	// buff_dmg: local unix timestamp
	// evtc: yes
	// realtime: yes

	CBTS_SQCOMBATEND, // squad combat stop, last player left combat. previously named log end
	// dst_agent: bit 0 = log ended by pov map exit
	// value: as uint32_t, server unix timestamp
	// buff_dmg: local unix timestamp
	// evtc: yes
	// realtime: yes

	CBTS_WEAPSWAP, // agent weapon set changed
	// src_agent: relates to agent
	// dst_agent: new weapon set id
	// value: old weapon seet id
	// evtc: yes
	// realtime: yes

	CBTS_MAXHEALTHUPDATE, // agent maximum health changed
	// src_agent: relates to agent
	// dst_agent: new max health
	// evtc: limited to non-players
	// realtime: no

	CBTS_POINTOFVIEW, // "recording" player
	// src_agent: relates to agent
	// evtc: yes
	// realtime: no

	CBTS_LANGUAGE, // text language id
	// src_agent: text language id
	// evtc: yes
	// realtime: no

	CBTS_GWBUILD, // game build
	// src_agent: game build number
	// evtc: yes
	// realtime: no

	CBTS_SHARDID, // server shard id
	// src_agent: shard id
	// evtc: yes
	// realtime: no

	CBTS_REWARD, // wiggly box reward
	// dst_agent: reward id
	// value: reward type
	// evtc: yes
	// realtime: yes

	CBTS_BUFFINITIAL, // buff application for buffs already existing at time of event
	// matches CBTS_BUFFAPPLY, except
	// buff_dmg: original ms duration of stack
	// evtc: limited to squad outside instances
	// realtime: limited to squad

	CBTS_POSITION, // agent position changed
	// src_agent: relates to agent
	// dst_agent: (float*)&dst_agent is float[3], x/y/z
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_VELOCITY, // agent velocity changed
	// src_agent: relates to agent
	// dst_agent: (float*)&dst_agent is float[3], x/y/z
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_FACING, // agent facing direction changed
	// src_agent: relates to agent
	// dst_agent: (float*)&dst_agent is float[2], x/y
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_TEAMCHANGE, // agent team id changed
	// src_agent: relates to agent
	// dst_agent: new team id
	// value: old team id
	// evtc: limited to agent table outside instances
	// realtime: limited to squad

	CBTS_ATTACKTARGET, // attacktarget to gadget association
	// src_agent: relates to agent, the attacktarget
	// dst_agent: the gadget
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_TARGETABLE, // agent targetable state
	// src_agent: relates to agent
	// dst_agent: new targetable state
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_MAPID, // map info
	// src_agent: map id
	// dst_agent: map type
	// evtc: yes
	// realtime: no

	CBTS_REPLINFO, // internal use
	// internal use

	CBTS_BUFFACTIVE, // buff instance is now active
	// src_agent: relates to agent
	// dst_agent: trackable id
	// value: current buff duration
	// evtc: limited to squad outside instances
	// realtime: limited to squad

	CBTS_BUFFDEACTIVE, // buff set inactive
	// src_agent: relates to agent
	// value: new duration
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: limited to squad outside instances
	// realtime: limited to squad

	CBTS_GUILD, // agent is a member of guild
	// src_agent: relates to agent
	// dst_agent: (uint8_t*)&dst_agent is uint8_t[16], guid of guild
	// value: new duration
	// evtc: limited to squad outside instances
	// realtime: no

	CBTS_BUFFINFO, // buff information. logs will always contain info for skill ids in skillid_mask below
	// skillid: skilldef id of buff
	// overstack_value: max combined duration
	// src_master_instid: 
	// is_src_flanking: likely an invuln
	// is_shields: likely an invert
	// is_offcycle: category
	// pad61: buff stacking type
	// pad62: likely a resistance
	// pad63: non-zero if used in buff damage simulation (rough pov-only check)
	// evtc: yes
	// realtime: no

	CBTS_BUFFFORMULA, // buff formula, one per event of this type. see bottom of this file for list of always-included skills
	// skillid: skilldef id of buff
	// time: (float*)&time is float[9], type attribute1 attribute2 parameter1 parameter2 parameter3 trait_condition_source trait_condition_self content_reference
	// src_instid: (float*)&src_instid is float[2], buff_condition_source buff_condition_self
	// evtc: yes
	// realtime: no

	CBTS_SKILLINFO, // skill information
	// skillid: skilldef id of skill
	// time: (float*)&time is float[4], cost range0 range1 tooltiptime
	// evtc: yes
	// realtime: no

	CBTS_SKILLTIMING, // skill timing, one per event of this type
	// skillid: skilldef id of skill
	// src_agent: timing type
	// dst_agent: at time since activation in milliseconds
	// evtc: yes
	// realtime: no

	CBTS_BREAKBARSTATE, // agent breakbar state changed
	// src_agent: relates to agent
	// dst_agent: new breakbar state
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_BREAKBARPERCENT, // agent breakbar percentage changed
	// src_agent: relates to agent
	// value: (float*)&value is float[1], new percentage
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_INTEGRITY, // one event per message. previously named error
	// time: (char*)&time is char[32], a short null-terminated message with reason
	// evtc: yes
	// realtime: no

	CBTS_MARKER, // one event per marker on an agent
	// src_agent: relates to agent
	// value: markerdef id. if value is 0, remove all markers presently on agent
	// buff: marker is a commander tag
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_BARRIERPCTUPDATE, // agent barrier percentage changed
	// src_agent: relates to agent
	// dst_agent: percent * 10000 eg. 99.5% will be 9950
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_STATRESET_DEFUNC, // retired, not used since 260402+
	// src_agent: species id of agent that triggered the reset, eg boss species id
	// evtc: yes
	// realtime: yes

	CBTS_EXTENSION, // for extension use. not managed by arcdps
	// evtc: yes
	// realtime: yes

	CBTS_APIDELAYED, // retired, not used since 260501+
	// retired

	CBTS_INSTANCESTART, // map instance start
	// src_agent: milliseconds ago instance was started
	// value: *(uint32_t*)&value server socket
	// evtc: yes
	// realtime: no

	CBTS_RATEHEALTH, // tick health. previously named tickrate
	// src_agent: 25 - tickrate, when tickrate <= 20 
	// evtc: yes
	// realtime: no

	CBTS_LAST90BEFOREDOWN, // retired, not used since 240529+
	// retired

	CBTS_EFFECT, // retired, not used since 230716+
	// retired

	CBTS_IDTOGUID, // content id to guid association for volatile types. types may contain additional information, see n_contentlocal
	// src_agent: (uint8_t*)&src_agent is uint8_t[16] guid of content
	// overstack_value: is of enum contentlocal
	// evtc: yes
	// realtime: no

	CBTS_LOGNPCUPDATE, // log boss agent changed
	// src_agent: species id of agent
	// dst_agent: related to agent
	// value: as uint32_t, server unix timestamp
	// evtc: yes
	// realtime: yes

	CBTS_IDLEEVENT, // internal use
	// internal use

	CBTS_EXTENSIONCOMBAT, // for extension use. not managed by arcdps
	// assumed to be cbtevent struct, skillid will be processed as such for purpose of buffinfo/skillinfo
	// evtc: yes
	// realtime: yes

	CBTS_FRACTALSCALE, // fractal scale for fractals
	// src_agent: scale
	// evtc: yes
	// realtime: no

	CBTS_EFFECT2_DEFUNC, // retired, not used since 250526+
	// src_agent: related to agent
	// dst_agent: effect at location of agent (if applicable)
	// value: (float*)&value is float[3], location x/y/z (if not at agent location)
	// iff: (uint32_t*)&iff is uint32_t[1], effect duration. if duration is zero, it may be a fixed length duration (see n_contentlocal)
	// buffremove: (uint32_t*)&buffremove is uint32_t[1], trackable id of effect. if dst_agent and location is 0(/0/0), effect was stopped
	// is_shields: (int16_t*)&is_shields is int16_t[3], orientation x/y/z, values are original*1000
	// is_flanking: effect is on a non-static platform
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_RULESET, // ruleset for self
	// src_agent: bit0: pve, bit1: wvw, bit2: pvp
	// evtc: yes
	// realtime: no

	CBTS_SQUADMARKER, // squad ground markers
	// src_agent: (float*)&src_agent is float[3], x/y/z of marker location. if values are all zero or infinity, this marker is removed
	// skillid: index of marker eg. 0 is arrow
	// evtc: yes
	// realtime: no

	CBTS_ARCBUILD, // arc build info
	// src_agent: (char*)&src_agent is a null-terminated string matching the full build string in arcdps.log
	// evtc: yes
	// realtime: no

	CBTS_GLIDER, // glider status change
	// src_agent: related to agent
	// value: 1 deployed, 0 stowed
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_STUNBREAK, // disable stopped early
	// src_agent: related to agent
	// value: duration remaining
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_MISSILECREATE, // create a missile
	// src_agent: related to agent
	// value: (int16*)&value is int16[3], location x/y/z, divided by 10
	// overstack_value: skin id (player only)
	// skillid: missile skill id
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_MISSILELAUNCH, // launch missile
	// src_agent: related to agent
	// dst_agent: at agent, if set and in range
	// value: (int16*)&value is int16[6], target x/y/z, current x/y/z, divided by 10
	// skillid: missile skill id
	// iff: (uint8_t*)&iff is uint8_t[1], launch motion. unknown, from client
	// result: (int16_t*)&result is int16[1], motion radius
	// is_buffremove: (uint32_t*)&is_buffremove is uint32_t[1], launch flags. unknown, from client
	// is_src_flanking: non-zero if first launch
	// is_shields: (int16_t*)&is_shields is int16[1], missile speed
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_MISSILEREMOVE, // remove missile
	// src_agent: related to agent
	// value: friendly fire damage total
	// skillid: missile skill id
	// is_src_flanking: hit at least one enemy along the way
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_EFFECTGROUNDCREATE, // play effect on ground
	// src_agent: related to agent
	// dst_agent: (int16*)&dst_agent is int16[6], origin x/y/z divided by 10, orient x/y/z multiplied by 1000
	// skillid: effect id (prefer using an id to guid map via n_contentlocal)
	// iff: (uint32_t*)&iff is uint32_t[1], effect duration. if duration is zero, it may be a fixed length duration (see n_contentlocal)
	// is_buffremove: flags
	// is_flanking: effect is on a non-static platform
	// is_shields: (int16_t*)&is_shields is int16[1], scale (if zero, assume 1) multiplied by 1000
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_EFFECTGROUNDREMOVE, // stop effect on ground
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: yes
	// realtime: no

	CBTS_EFFECTAGENTCREATE, // play effect around agent
	// src_agent: related to agent
	// skillid: effect id (prefer using an id to guid map via n_contentlocal)
	// iff: (uint32_t*)&iff is uint32_t[1], effect duration. if duration is zero, it may be a fixed length duration (see n_contentlocal)
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_EFFECTAGENTREMOVE, // stop effect around agent
	// src_agent: related to agent
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_IIDCHANGE, // iid (previously evtc_agent->addr) changed. players only, happens after spawn when player historical data is loaded. does not happen if player has no historical data
	// src_agent: old iid
	// dst_agent: new iid
	// evtc: yes
	// realtime: no

	CBTS_MAPCHANGE, // map changed
	// src_agent: new map id
	// dst_agent: old map id
	// value: new map type
	// evtc: yes
	// realtime: yes

	CBTS_EARLYEXIT, // internal use
	// internal use

	CBTS_ANIMATIONSTART, // animation start
	// src_agent: agent beginning animation
	// dst_agent: target agent if applicable
	// value: ms duration until minimum of last significant trigger point and tooltip time
	// buff_dmg: ms duration when control is returned to agent
	// overstack_value: reference id (emote id if CSK_EMOTE)
	// skillid: skill id
	// result: activation start source (debug)
	// evtc: yes
	// realtime: yes

	CBTS_ANIMATIONSTOP, // animation stop
	// src_agent: agent beginning animation
	// value: ms duration spent in animation scaled for speed
	// buff_dmg: ms duration spent in animation not scaled
	// skillid: skill id of previous animation start
	// is_activation: simple progress check from cbtanimation
	// result: activation stop source (debug)
	// evtc: yes
	// realtime: yes

	CBTS_BUFFAPPLY, // buff stack application
	// src_agent: agent applying the stack
	// dst_agent: agent the stack was applied to
	// value: ms duration applied
	// skillid: buff skill id
	// is_shields: non-zero if buff is active when applied
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: yes
	// realtime: limited to visible, must have previous squad to squad application

	CBTS_BUFFCHANGE, // buff stack duration change, active only
	// dst_agent: relates to agent
	// value: duration difference
	// skillid: buff skill id
	// overstack_value: new ms duration
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: yes
	// realtime: limited to visible, must have previous squad to squad application

	CBTS_BUFFREMOVE_SINGLE, // buff stack removed
	// src_agent: agent with buff removed
	// dst_agent: agent removing the buff
	// value: ms duration removed
	// skillid: buff skill id
	// is_buffremove: of enum cbtbuffremove
	// pad61: (uint32_t*)&pad61 is uint32[1], trackable id
	// evtc: yes
	// realtime: limited to visible, must have previous squad to squad application

	CBTS_BUFFREMOVE_ALL, // all buff stacks of skillid removed
	// src_agent: agent with buffs removed
	// dst_agent: agent removing the buffs
	// value: ms duration removed calculated as duration
	// buff_dmg: ms duration removed calculated as intensity
	// skillid: buff skill id
	// is_buffremove: of enum cbtbuffremove
	// evtc: yes
	// realtime: limited to visible, must have previous squad to squad application

	CBTS_TRANSFORMATION, // transformation
	// src_agent: related to agent
	// skillid: transformation id (0 if untransform)
	// evtc: limited to agent table outside instances
	// realtime: no

	CBTS_UNKNOWN // unknown/unsupported type newer than this list perhaps
};

/* combat buff remove type */
enum cbtbuffremove {
	CBTB_NONE, // not used - not this kind of event
	CBTB_ALL, // last/all stacks removed (sent by server)
	CBTB_SINGLE, // single stack removed (sent by server)
	CBTB_MANUAL, // single stack removed (created by arc on all stack remove)
	CBTB_UNKNOWN
};

/* custom skill ids */
enum cbtcustomskill {
	CSK_RESURRECT = 1066, // not custom but important and unnamed
	CSK_BANDAGE = 1175, // personal healing only
	CSK_DODGE = 65001 // will occur in is_activation==normal event
};

/* language */
enum gwlanguage {
	GWL_ENG = 0,
	GWL_FRE = 2,
	GWL_GEM = 3,
	GWL_SPA = 4,
};

/* combat event logging (revision 1, when header[12] == 1) */
typedef struct cbtevent {
	uint64_t time;
	uint64_t src_agent;
	uint64_t dst_agent;
	int32_t value;
	int32_t buff_dmg;
	uint32_t overstack_value;
	uint32_t skillid;
	uint16_t src_instid;
	uint16_t dst_instid;
	uint16_t src_master_instid;
	uint16_t dst_master_instid;
	uint8_t iff;
	uint8_t buff;
	uint8_t result;
	uint8_t is_activation;
	uint8_t is_buffremove;
	uint8_t is_ninety;
	uint8_t is_fifty;
	uint8_t is_moving;
	uint8_t is_statechange;
	uint8_t is_flanking;
	uint8_t is_shields;
	uint8_t is_offcycle;
	uint8_t pad61;
	uint8_t pad62;
	uint8_t pad63;
	uint8_t pad64;
} cbtevent;

/* agent short */
typedef struct ag
{
	char* name; /* agent name. may be null. valid only at time of event. utf8 */
	uintptr_t id; /* agent unique identifier */
	uint32_t prof; /* profession at time of event. refer to evtc notes for identification */
	uint32_t elite; /* elite spec at time of event. refer to evtc notes for identification */
	uint32_t self; /* 1 if self, 0 if not */
	uint16_t team; /* sep21+ */
} ag;

const unsigned int combat_api_delay = 3000;//delay from realtime, guess, in ms
const unsigned int ms_per_tick = 40;// 1000ms/25hz