#include "helpers.h"

#include "imgui/imgui.h"

bool isPlayer(const ag* new_player) noexcept
{
	return new_player
		&& new_player->elite != 0xffffffff
		&& new_player->name
		&& new_player->id;
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
void showHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}