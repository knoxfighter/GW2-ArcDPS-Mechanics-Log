#pragma once

#include <mutex>
#include <list>
#include <string>
#include <utility>

#include "player.h"
#include "mechanics.h"
#include "npc_ids.h"
#include "helpers.h"
#include "LogEvent.h"
#include "MechanicEntry.h"
#include "PlayerEntry.h"

class Tracker
{
public:
	std::mutex players_mtx;
	std::mutex tracker_mtx;
	std::mutex log_events_mtx;
	std::mutex mechanic_entries_mtx;
	
	std::list<Player> players;
	
	std::list<LogEvent> log_events;
	int max_log_events = 300;
	
	bool show_only_self = false;
	bool export_chart_on_close = true;

	std::list<PlayerEntry> player_entries;
	
	Boss* boss_data = nullptr;
	uint64_t current_log_npc = 0;
	int64_t start_time = 0;

	PlayerEntry* getPlayerEntry(const ag* new_player);
	PlayerEntry* getPlayerEntry(uintptr_t new_player);
	PlayerEntry* getPlayerEntry(std::string new_player);
	bool addPlayer(ag* src, ag* dst);//src&dst of combat event
	bool removePlayer(const ag* src);

	void addPull(Boss* boss);
	void resetAllPlayerStats();
	void clearLog();
	uint16_t getMechanicsTotal();
	uint8_t getPlayerNumInCombat();

	void processCombatEnter(const cbtevent* ev, ag* new_agent);
	void processCombatExit(const cbtevent* ev, ag* new_agent);
	void processLogNpcUpdate(uint64_t species_id);
	void processMechanic(const cbtevent* ev, PlayerEntry* new_player_src, PlayerEntry* new_player_dst, Mechanic* new_mechanic, int64_t value);

	int Tracker::getElapsedTime(uint64_t const &current_time) noexcept;
};
