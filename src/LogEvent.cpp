#include "LogEvent.h"
#include "imgui/imgui.h"

LogEvent::LogEvent(Player* new_player, Mechanic* new_mechanic, uint64_t new_time, uint64_t new_time_absolute, int64_t new_value, const cbtevent* new_ev, int max_display_name_length)
{
	player = new_player;
	mechanic = new_mechanic;
	time = new_time;
	time_absolute = new_time_absolute;
	value = new_value;
	
	if (new_ev)ev = *new_ev;

	bakeStr(max_display_name_length);
}

void LogEvent::draw()
{
	if (isPlaceholder())
	{
		ImGui::Separator();
		return;
	}

	ImGui::TextUnformatted(str.c_str());
}

void LogEvent::drawTooltip()
{
	if (!player) return;

	ImGui::BeginTooltip();
	
	ImGui::Text("Event time: %d", ev.time);
	ImGui::Text("Source: ");
	ImGui::Text("Destination: ");
	ImGui::Text("Skill ID: %d", ev.skillid);

	ImGui::EndTooltip();
}

void LogEvent::bakeStr(int max_display_name_length)
{
	if (isPlaceholder())
	{
		return;
	}
	
	std::string output = "";
	
	if (time < 0)
	{
		output += "-";
	}
	output += std::to_string(abs(time / 60));
	output += ":";
	if (abs(time) % 60 < 10)
	{
		output += "0";
	}
	output += std::to_string(abs(time) % 60);

	output += " - ";
	
	output += player ? ((std::string_view)player->name).substr(0, max_display_name_length) : "Unknown Player";

	output += " ";
	output += mechanic->name;

	if (value != 1)
	{
		output += ", value = " + std::to_string(value);
	}

	str = output;
}

bool LogEvent::isPlaceholder()
{
	return !player && !mechanic;
}

std::string LogEvent::getFilterText()
{
	if (isPlaceholder())
	{
		return "=";
	}
	return str;
}
