#include "MechanicFilter.h"



MechanicFilter::MechanicFilter()
{
}


MechanicFilter::~MechanicFilter()
{
}

static auto  getMechanicNamesForComboBox = [](void* current_mechanics,int current_item, const char** item_text)
{
	auto& vector = *static_cast<std::vector<Mechanic>*>(current_mechanics);
	*item_text = vector.at(current_item).name_chart.c_str();
	return true;
};

void MechanicFilter::drawPopup()
{
	filter_player.Draw("Player");
	ImGui::Separator();
	filter_boss.Draw("Boss");
	ImGui::Separator();
	filter_mechanic.Draw("Mechanic");

	ImGui::Checkbox("Only show players currently in squad", &show_in_squad_only);
	ImGui::Checkbox("Show all mechanics", &show_all_mechanics);
	ImGui::SameLine(); showHelpMarker("Display all mechanics regardless of default options. Can be spammy.");
}

bool MechanicFilter::isActive()
{
	return filter_player.IsActive() || filter_boss.IsActive() || filter_mechanic.IsActive() || show_in_squad_only;
}

bool MechanicFilter::passFilter(Player* new_player, Boss* new_boss, Mechanic* new_mechanic, Verbosity new_display_section)
{
	if (new_player)
	{
		if (!filter_player.PassFilter(new_player->name.c_str())) return false;
		
		if (show_in_squad_only && !new_player->in_squad) return false;
	}
	if (new_boss)
	{
		if (!filter_boss.PassFilter(new_boss->name.c_str())) return false;
	}
	if (!show_all_mechanics)
	{
		if (new_mechanic && !((int)new_mechanic->verbosity & (int)new_display_section)) return false;
	}
	if (new_mechanic)
	{
		if (!filter_mechanic.PassFilter(new_mechanic->name.c_str())) return false;
		if (new_mechanic->boss)
		{
			if (!filter_boss.PassFilter(new_mechanic->boss->name.c_str())) return false;
		}
	}
	
	return true;
}

bool MechanicFilter::passFilter(LogEvent* new_event)
{
	if (!new_event) return false;

	Player* current_player = new_event->player;

	Boss* current_boss = (new_event->mechanic ? new_event->mechanic->boss : nullptr);

	Mechanic* current_mechanic = new_event->mechanic;

	return passFilter(current_player, current_boss, current_mechanic,Verbosity::Log);
}
