#include "Tracker.h"

PlayerEntry* Tracker::getPlayerEntry(const ag* new_player)
{
	if (!isPlayer(new_player)) return nullptr;

	std::lock_guard<std::mutex> lg(players_mtx);

	auto it = std::find(player_entries.begin(), player_entries.end(), new_player->id);

	//player not tracked yet
	if (it == player_entries.end())
	{
		return nullptr;
	}
	else//player tracked
	{
		return &*it;
	}
}

PlayerEntry * Tracker::getPlayerEntry(uintptr_t new_player)
{
	if (!new_player) return nullptr;

	std::lock_guard<std::mutex> lg(players_mtx);

	auto it = std::find(player_entries.begin(), player_entries.end(), new_player);

	//player not tracked yet
	if (it == player_entries.end())
	{
		return nullptr;
	}
	else//player tracked
	{
		return &*it;
	}
}

PlayerEntry * Tracker::getPlayerEntry(std::string new_player)
{
	if (new_player.empty()) return nullptr;

	std::lock_guard<std::mutex> lg(players_mtx);

	auto it = std::find(player_entries.begin(), player_entries.end(), new_player);

	//player not tracked yet
	if (it == player_entries.end())
	{
		return nullptr;
	}
	else//player tracked
	{
		return &*it;
	}
}

bool Tracker::addPlayer(ag* src, ag* dst)
{
	if (!src) return false;
	if (!dst) return false;

	char* name = src->name;
	char* account = dst->name;
	const uintptr_t id = src->id;
	bool is_self = dst->self;

	if (!name) return false;
	if (!account) return false;
	if (std::string(name).length() < 2) return false;
	if (std::string(account).length() < 2) return false;

	PlayerEntry* new_entry = getPlayerEntry(account);
	
	//player not tracked yet

	std::lock_guard<std::mutex> lg(players_mtx);

	if (!new_entry)
	{
		players.push_back(Player(name, account, id, is_self));
		players.back().current_log_npc = &current_log_npc;
		player_entries.push_back(PlayerEntry(&players.back()));
	}
	else//player tracked
	{
		new_entry->player->id = id;
		new_entry->player->name = name;
		new_entry->player->in_squad = true;
		new_entry->player->is_self = is_self;
	}
	return true;
}

bool Tracker::removePlayer(const ag* src)
{
	if (!src) return false;

	const char* name = src->name;
	const char* account = src->name;//TODO: if account names are ever added, put it here
	const uintptr_t id = src->id;

	PlayerEntry* new_entry = getPlayerEntry(id);

	//player not tracked yet

	std::lock_guard<std::mutex> lg(players_mtx);

	if (!new_entry)
	{
		return false;
	}
	else
	{
		new_entry->player->in_squad = false;
		return true;
	}
}

void Tracker::addPull(Boss* boss)
{
	if (!boss) return;
	
	boss->pulls++;

	for (auto current_entry = player_entries.begin(); current_entry != player_entries.end(); ++current_entry)
	{
		current_entry->addPull(boss);
	}
}

void Tracker::resetAllPlayerStats()
{
	std::lock_guard<std::mutex> lg(players_mtx);
	player_entries.clear();
	players.clear();
}

void Tracker::clearLog()
{
	std::lock_guard<std::mutex> lg(log_events_mtx);

	log_events.clear();
	has_logged_mechanic = false;
}

uint16_t Tracker::getMechanicsTotal()
{
	uint16_t result = 0;

	for (auto current_entry = player_entries.begin(); current_entry != player_entries.end(); ++current_entry)
	{
		result += current_entry->getMechanicsTotal();
	}
	return result;
}

uint8_t Tracker::getPlayerNumInCombat()
{
	uint8_t result = 0;

	for (auto player = players.begin(); player != players.end(); ++player)
	{
		if (player->in_squad && player->in_combat)
		{
			result++;
		}
	}
	return result;
}

void Tracker::processCombatEnter(const cbtevent* ev, ag* new_agent)
{
	PlayerEntry* new_entry = nullptr;
	if (new_entry = getPlayerEntry(new_agent))
	{
		Player* new_player = new_entry->player;

		new_entry->combatEnter();

		if (new_agent && new_agent->self)
		{
			start_time = ev->time;

			if (log_events.size() > 1
				&& (ev->time - log_events.back().time_absolute) < combat_api_delay)
			{//if a mechanic happens when the fight starts, it's probably part of the new fight and not the old one
				log_events.back().time = getElapsedTime(ev->time);
				log_events.back().bakeStr();
			}

			if (has_logged_mechanic)
			{
				has_logged_mechanic = false;

				if (log_events.size() > 1
					&& (ev->time - log_events.back().time_absolute) < combat_api_delay)
				{//if a mechanic happens when the fight starts, ensure the separator is before the mechanic
					log_events.insert(--log_events.end(), LogEvent(nullptr, nullptr, getElapsedTime(ev->time), ev->time, 1,nullptr));
				}
				else
				{
					log_events.push_back(LogEvent(nullptr, nullptr, getElapsedTime(ev->time), ev->time, 1,nullptr));//TODO: make function for pushing log events
				}

				if (log_events.size() > max_log_events)
				{
					log_events.pop_front();
				}
			}
		}
	}

	if (!boss_data && new_agent)
	{
		for (auto current_boss = bosses.begin(); current_boss != bosses.end(); ++current_boss)
		{
			if ((*current_boss)->hasId(new_agent->prof))
			{
				boss_data = *current_boss;
			}
		}
	}
}

void Tracker::processCombatExit(const cbtevent* ev, ag* new_agent)
{
	PlayerEntry* new_entry = nullptr;
	if (new_entry = getPlayerEntry(new_agent))
	{
		Player* new_player = new_entry->player;

		new_entry->combatExit();

		if (getPlayerNumInCombat() == 0)
		{
			addPull(boss_data);
			boss_data = nullptr;
			current_log_npc = 0;
		}
	}
}

void Tracker::processLogNpcUpdate(uint64_t species_id)
{
	current_log_npc = species_id;
}

void Tracker::processMechanic(const cbtevent* ev, PlayerEntry* new_player_src, PlayerEntry* new_player_dst, Mechanic* new_mechanic, int64_t value)
{
	std::lock_guard<std::mutex> lg(tracker_mtx);

	PlayerEntry* relevant_entry = new_mechanic->target_is_dst ? new_player_dst : new_player_src;

	if (!relevant_entry) return;

	if (new_mechanic->is_multihit && ev->time < (relevant_entry->getLastMechanicHitTime(new_mechanic->ids[0]) + new_mechanic->frequency_player)) return;
		
	std::lock_guard<std::mutex> lg2(log_events_mtx);
		
	log_events.push_back(LogEvent(relevant_entry->player, new_mechanic, getElapsedTime(ev->time), ev->time, value, ev));
		
	if (log_events.size() > max_log_events)
	{
		log_events.pop_front();
	}

	has_logged_mechanic = true;
	
	relevant_entry->addMechanicEntry(ev->time, new_mechanic, new_mechanic->boss);
}

int Tracker::getElapsedTime(uint64_t const &current_time) noexcept
{
	return (current_time - start_time) / 1000;
}
