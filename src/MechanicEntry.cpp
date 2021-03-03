#include "MechanicEntry.h"

MechanicEntry::MechanicEntry(uint64_t new_time, Mechanic * new_mechanic, Player * new_player, const Boss * new_boss)
{
	player = new_player;
	mechanic = new_mechanic;
	current_boss = new_boss;
	hits = 1;
	pulls = new_boss ? new_boss->pulls : 1;
	last_hit_time = new_time;
}

std::string MechanicEntry::toString()
{
	if (!isRelevant()) return "";

	return
		(current_boss ? current_boss->name : "") + "," +
		mechanic->getName() + "," +
		(!mechanic->fail_if_hit ? std::to_string(hits) + "," : "," + std::to_string(hits)) +
		"," + "," + "," +
		std::to_string(pulls) +
		"\n";
}

void MechanicEntry::addPull(const Boss * new_boss)
{
	if (current_boss == new_boss)
	{
		pulls++;
	}
}

bool MechanicEntry::isRelevant()
{
	return hits > 0;
}
