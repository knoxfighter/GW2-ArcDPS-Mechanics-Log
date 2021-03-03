#include "imgui_panels.h"

#include <Windows.h>
#include <ctime>
#include <fstream>
#include <ShlObj.h>

void AppLog::draw(const char* title, bool* p_open, ImGuiWindowFlags flags, Tracker* tracker)
{
    ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
	ImGui::Begin(title, p_open, flags);
	ImGui::BeginChild("Buttons",ImVec2(0, ImGui::GetFrameHeightWithSpacing()));
	if (ImGui::Button("Clear")) tracker->clearLog();;
    ImGui::SameLine();
    const bool copy = ImGui::Button("Copy");
    ImGui::SameLine();

	std::string filter_button_text = "Filter - " 
		+ (filter.isActive() ? std::string("Active") : std::string("Inactive"));

	if (ImGui::Button(filter_button_text.c_str(),ImVec2(-1,0))) ImGui::OpenPopup("FilterOptions");
	if (ImGui::BeginPopup("FilterOptions"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		filter.drawPopup();
		ImGui::Checkbox("Show separators between pulls", &show_pull_separators);
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	
	ImGui::EndChild();
    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
	int64_t last_mechanic_time = 0;
	bool beginning = true;
	bool previous_was_placeholder = true;
	if (copy) ImGui::LogToClipboard();

	std::lock_guard<std::mutex> lg(tracker->log_events_mtx);

	for (auto current_event = tracker->log_events.begin(); current_event != tracker->log_events.end(); ++current_event)
	{
		if (current_event->player
			&& !current_event->player->is_self
			&& tracker->show_only_self)
			continue;

		if (!filter.passFilter(&*current_event)) continue;

		if (!show_pull_separators && current_event->isPlaceholder()) continue;
		if (previous_was_placeholder && current_event->isPlaceholder()) continue; //don't show multiple placeholders if nothing is between them

		if (!beginning
			&& current_event->time_absolute > (last_mechanic_time + line_break_frequency))
		{
			ImGui::Dummy(ImVec2(0, ImGui::GetTextLineHeight()));
		}
		last_mechanic_time = current_event->time_absolute;
			
		current_event->draw();
//		if (ImGui::IsItemHovered() && !current_event->isPlaceholder()) current_event->drawTooltip();

		beginning = false;
		previous_was_placeholder = current_event->isPlaceholder();
    }

    if (scroll_to_bottom)
        ImGui::SetScrollHere(1.0f);
	scroll_to_bottom = false;
    ImGui::EndChild();
    ImGui::End();
}

void AppChart::clear(Tracker* tracker)
{
	if (!tracker) return;

	tracker->resetAllPlayerStats();
}

void AppChart::draw(Tracker* tracker, const char* title, bool* p_open, ImGuiWindowFlags flags, bool show_all)
{
    ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
    ImGui::Begin(title, p_open, flags);

    const float window_width = ImGui::GetWindowContentRegionWidth();
	int current_mechanic_fail_count = 0;//number of mechanics for a given player given current filter settings
	int current_mechanic_neutral_count = 0;

	Player* current_player = nullptr;

    if (ImGui::Button("Clear")) clear(tracker);
    ImGui::SameLine();
    if (ImGui::Button("Export")) exportData(tracker);
    ImGui::SameLine();
    if (ImGui::Button("Copy")) ImGui::LogToClipboard();
	ImGui::SameLine();

	std::string filter_button_text = "Filter - "
		+ (filter.isActive() ? std::string("Active") : std::string("Inactive"));

	if (ImGui::Button(filter_button_text.c_str(), ImVec2(-1, 0))) ImGui::OpenPopup("FilterOptions");
	if (ImGui::BeginPopup("FilterOptions"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		filter.drawPopup();
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}

	
    ImGui::Separator();

    ImGui::BeginGroup();
    ImGui::Text("Name");
    ImGui::SameLine(getChartColumnLoc(window_width,1));
    ImGui::Text("Neutral");
    ImGui::SameLine(getChartColumnLoc(window_width,2));
    ImGui::Text("Failed");
    ImGui::SameLine(getChartColumnLoc(window_width,3));
    ImGui::Text("Downs");
    ImGui::SameLine(getChartColumnLoc(window_width,4));
    ImGui::Text("Deaths");
    ImGui::SameLine(getChartColumnLoc(window_width,5));
    ImGui::Text("Pulls");
    ImGui::EndGroup();

    ImGui::BeginChild("scrolling");
	ImGui::PushItemWidth(window_width*0.9);
	for (auto current_entry = tracker->player_entries.begin(); current_entry != tracker->player_entries.end(); ++current_entry)
    {
		current_player = current_entry->player;
		if (!current_player) continue;

		if (!current_player->is_self
			&& tracker->show_only_self)
			continue;

		if (!filter.passFilter(current_player,nullptr,nullptr,Verbosity::All))
			continue;

		ImGui::Separator();
        ImGui::GetFrameHeightWithSpacing();
        ImGui::Text(current_player->name_account_combo.c_str());

		for (auto current_player_mechanics = current_entry->entries.begin(); current_player_mechanics != current_entry->entries.end(); ++current_player_mechanics)
        {
			if (!current_player_mechanics->isRelevant()) continue;
			if (!current_player_mechanics->mechanic) continue;
			if (!filter.passFilter(current_player, nullptr, current_player_mechanics->mechanic,Verbosity::Chart)) continue;

            ImGui::Indent();
            ImGui::Text(current_player_mechanics->mechanic->getChartName().c_str());
            if(!current_player_mechanics->mechanic->fail_if_hit)
            {
                ImGui::SameLine(getChartColumnLoc(window_width,1));
				current_mechanic_neutral_count += current_player_mechanics->hits;
            }
            else
            {
                ImGui::SameLine(getChartColumnLoc(window_width,2));
				current_mechanic_fail_count += current_player_mechanics->hits;
            }
            ImGui::Text("%d", current_player_mechanics->hits);
            ImGui::SameLine(getChartColumnLoc(window_width,5));
            ImGui::Text("%d", current_player_mechanics->pulls);
            ImGui::Unindent();
                

            ImGui::Separator();
        }

		ImGui::Indent();
		ImGui::Text("Total (with current filters)");

		ImGui::SameLine(getChartColumnLoc(window_width, 1));
		ImGui::Text("%d", current_mechanic_neutral_count);
		ImGui::SameLine(getChartColumnLoc(window_width, 2));
		ImGui::Text("%d", current_mechanic_fail_count);
		ImGui::SameLine(getChartColumnLoc(window_width, 3));
		ImGui::Text("%d", current_entry->downs);
		ImGui::SameLine(getChartColumnLoc(window_width, 4));
		ImGui::Text("%d", current_entry->deaths);
		ImGui::SameLine(getChartColumnLoc(window_width, 5));
		ImGui::Text("%d", current_entry->pulls);
		ImGui::Unindent();
		ImGui::Separator();

		current_mechanic_neutral_count = 0;
		current_mechanic_fail_count = 0;
    }
	ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::End();
}

constexpr float getChartColumnWidth(float window_width)
{
    return window_width/6.0*3.0/5.0;
}

float getChartColumnLoc(float window_width, uint16_t col)
{
     return (window_width/3.0) + col * getChartColumnWidth(window_width);
}

std::string AppChart::toString(Tracker* tracker)
{
    std::string output = "";

    output += "Player Name,Account Name,Boss Name,Mechanic Name,Neutral,Failed,Downs,Deaths,Pulls\n";

	for (auto current_entry = tracker->player_entries.begin(); current_entry != tracker->player_entries.end(); ++current_entry)
    {
        if(current_entry->isRelevant())
        {
            output += current_entry->toString();
        }
    }
    return output;
}

void AppChart::exportData(Tracker* tracker)
{
	writeToDisk(tracker);

	ShellExecuteA(nullptr, "open", last_file_path.c_str(), nullptr, export_dir.c_str(), SW_HIDE);
}

void AppChart::writeToDisk(Tracker* tracker)
{
	if (!tracker) return;

    const uint16_t new_export_total = tracker->getMechanicsTotal();
    if(last_export_total == new_export_total || new_export_total < 2)
    {
        return;
    }

	std::string text = toString(tracker);

    const std::time_t t = std::time(nullptr);
    char time_str[100];
    if (std::strftime(time_str, sizeof(time_str), "%Y%m%d-%H%M%S", std::localtime(&t)))
    {
        //std::cout << time_str << '\n';
    }

    CreateDirectory(export_dir.c_str(), nullptr);

	std::string file_path = export_dir + "\\" + std::string(time_str) + "-" + std::to_string(new_export_total) + ".csv";

    std::ofstream out(file_path);
    out << text;
    out.close();

    last_export_total = new_export_total;

	last_file_path = file_path;
}

std::string AppChart::getDefaultExportPath()
{
	CHAR my_documents[MAX_PATH];
	const HRESULT result = SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, my_documents);
	if (result != S_OK)
	{
		//std::cout << "Error: " << result << "\n";
	}
	else
	{
		return std::string(my_documents) + "\\Guild Wars 2\\addons\\arcdps\\arcdps.mechanics";
	}
	return "";
}

void AppOptions::draw(Tracker* tracker, const char * title, bool * p_open, ImGuiWindowFlags flags)
{
	ImGui::SetNextWindowSize(ImVec2(550, 650), ImGuiCond_FirstUseEver);
	ImGui::Begin(title, p_open, flags);
	
	ImGui::Checkbox("Only show mechanics for self", &tracker->show_only_self);

	ImGui::InputInt("Max mechanics in log", &tracker->max_log_events, 25);

	ImGui::Checkbox("Export chart to CSV when game is closed", &tracker->export_chart_on_close);

	ImGui::Separator();
	
	ImGui::Text("Where to show each mechanic");

	ImGui::PushItemWidth(ImGui::GetWindowWidth()/3.0f);

	Boss* previous_boss = nullptr;

	for (auto current_mechanic = getMechanics().begin(); current_mechanic != getMechanics().end(); ++current_mechanic)
	{
		if(previous_boss && previous_boss != current_mechanic->getBoss())
			ImGui::Separator();

		ImGui::Combo(current_mechanic->getChartName().c_str(), (int*)&(current_mechanic->verbosity),
			"Hidden\0"
			"Chart Only\0"
			"Log only\0"
			"Everywhere\0\0",4);

		if (current_mechanic->description.length() > 0)
		{
			ImGui::SameLine(); showHelpMarker(current_mechanic->description.c_str());
		}

		previous_boss = current_mechanic->getBoss();
	}
	ImGui::PopItemWidth();

	ImGui::End();
}
