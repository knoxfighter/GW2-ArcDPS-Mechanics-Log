#include "PlayerEntry.h"

PlayerEntry::PlayerEntry(Player * new_player)
{
	player = new_player;
}

void PlayerEntry::addMechanicEntry(uint64_t new_time, Mechanic * new_mechanic, const Boss * new_boss)
{
	if (!new_mechanic) return;

	if (new_mechanic->fail_if_hit)
	{
		mechanics_failed++;
	}
	else
	{
		mechanics_neutral++;
	}

	last_hit_time = new_time;

	auto it = std::find(entries.begin(), entries.end(), new_mechanic->ids[0]);

	//mechanic not tracked yet
	if (it == entries.end())
	{
		entries.push_back(MechanicEntry(new_time, new_mechanic, player, new_boss));
	}
	else//mechanic tracked
	{
		it->hits++;

		if (it->last_hit_time < new_time)it->last_hit_time = new_time;
	}
}

void PlayerEntry::addPull(const Boss * new_boss)
{
	if (!player->in_squad) return;
	if (!new_boss) return;

	pulls++;

	for (auto current_entry = entries.begin(); current_entry != entries.end(); ++current_entry)
	{
		current_entry->addPull(new_boss);
	}
}

int PlayerEntry::getMechanicsTotal()
{
	uint16_t result = 1 + downs + deaths;

	for (auto current_entry = entries.begin(); current_entry != entries.end(); ++current_entry)
	{
		result += current_entry->hits;
	}
	return result;
}

uint64_t PlayerEntry::getLastMechanicHitTime(uint32_t new_mechanic)
{
	for (auto current_entry = entries.begin(); current_entry != entries.end(); ++current_entry)
	{
		if (current_entry->mechanic->ids[0] == new_mechanic)
		{
			return current_entry->last_hit_time;
		}
	}
	return 0;
}

void PlayerEntry::down()
{
	downs++;
	player->is_downed = true;
}

void PlayerEntry::dead()
{
	deaths++;
}

void PlayerEntry::rally()
{
	player->is_downed = false;
}

void PlayerEntry::fixDoubleDown()
{
	if (downs > 0) downs--;
}

bool PlayerEntry::isRelevant()
{
	return mechanics_neutral > 0
		|| mechanics_failed > 0
		|| player->in_squad;
}

std::string PlayerEntry::toString()
{
	std::string output = "";

	output += player->name + "," +
		player->account + "," +
		"All" + "," +
		"Overall" + "," +
		std::to_string(mechanics_neutral) + "," +
		std::to_string(mechanics_failed) + "," +
		std::to_string(downs) + "," +
		std::to_string(deaths) + "," +
		std::to_string(pulls) +
		"\n";

	for (auto current_entry = entries.begin(); current_entry != entries.end(); ++current_entry)
	{
		output += player->name + "," +
			player->account + "," +
			current_entry->toString();
	}

	return output;
}

void PlayerEntry::combatEnter()
{
	player->in_combat = true;
}

void PlayerEntry::combatExit()
{
	player->in_combat = false;
}

void PlayerEntry::setStabTime(uint64_t new_stab_time)
{
	if (player->last_stab_time < new_stab_time)
	{
		player->last_stab_time = new_stab_time;
	}
}