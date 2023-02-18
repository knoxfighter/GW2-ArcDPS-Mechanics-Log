#include <Windows.h>
#include <string>
#include <vector>
#include <cstdint>

#include "arcdps_datastructures.h"
#include "helpers.h"
#include "imgui_panels.h"
#include "mechanics.h"
#include "player.h"
#include "skill_ids.h"
#include "Tracker.h"
#include "imgui/imgui.h"
#include "simpleini\SimpleIni.h"

/* proto/globals */
arcdps_exports arc_exports;
char* arcvers;
void dll_init(HANDLE hModule);
void dll_exit();
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversionstr, ImGuiContext* imguicontext, void* id3dd9, HMODULE new_arcdll, void* mallocfn, void* freefn);
extern "C" __declspec(dllexport) void* get_release_addr();

arcdps_exports* mod_init();
uintptr_t mod_release();
uintptr_t mod_wnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision);
uintptr_t mod_imgui(uint32_t not_charsel_or_loading);
uintptr_t mod_options_end();
uintptr_t mod_options_windows(const char* windowname);

static int changeExportPath(const ImGuiInputTextCallbackData* data);
void readArcExports();
void parseIni();
void writeIni();
bool modsPressed();
bool canMoveWindows();
bool canClickWindows();

typedef uint64_t(*arc_export_func_u64)();
HMODULE arc_dll;;
arc_export_func_u64 arc_export_e6;
bool arc_hide_all = false;
bool arc_panel_always_draw = false;
bool arc_movelock_altui = false;
bool arc_clicklock_altui = false;
bool arc_window_fastclose = false;

arc_export_func_u64 arc_export_e7;
DWORD arc_global_mod1 = 0;
DWORD arc_global_mod2 = 0;
DWORD arc_global_mod_multi = 0;

bool show_app_log = false;
AppLog log_ui;

bool show_app_chart = false;
AppChart chart_ui;

bool show_options = false;
AppOptions options_ui;

Tracker tracker;

CSimpleIniA mechanics_ini(true);
bool valid_mechanics_ini = false;
WPARAM log_key;
WPARAM chart_key;


/* dll main -- winapi */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ulReasonForCall, LPVOID lpReserved) {
	return TRUE;
}

/* export -- arcdps looks for this exported function and calls the address it returns */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversionstr, ImGuiContext* imguicontext, void* id3dd9, HMODULE new_arcdll, void* mallocfn, void* freefn) {
	arcvers = arcversionstr;
	ImGui::SetCurrentContext(imguicontext);
	ImGui::SetAllocatorFunctions(static_cast<void*(*)(size_t sz, void* user_data)>(mallocfn), static_cast<void(*)(void* ptr, void* user_data)>(freefn));

	arc_dll = new_arcdll;
	arc_export_e6 = (arc_export_func_u64)GetProcAddress(arc_dll, "e6");
	arc_export_e7 = (arc_export_func_u64)GetProcAddress(arc_dll, "e7");
	return mod_init;
}

/* export -- arcdps looks for this exported function and calls the address it returns */
extern "C" __declspec(dllexport) void* get_release_addr() {
	arcvers = nullptr;
	return mod_release;
}

/* initialize mod -- return table that arcdps will use for callbacks */
arcdps_exports* mod_init()
{
	/* for arcdps */
	memset(&arc_exports, 0, sizeof(arcdps_exports));
	arc_exports.sig = 0x81004122;//from random.org
	arc_exports.imguivers = IMGUI_VERSION_NUM;
	arc_exports.size = sizeof(arcdps_exports);
	arc_exports.out_name = "Mechanics";
	arc_exports.out_build = __DATE__ " " __TIME__;
	arc_exports.wnd_nofilter = mod_wnd;
	arc_exports.combat = mod_combat;
	arc_exports.imgui = mod_imgui;
	arc_exports.options_end = mod_options_end;
	arc_exports.options_windows = mod_options_windows;

	parseIni();

	return &arc_exports;
}

/* release mod -- return ignored */
uintptr_t mod_release()
{
	if(tracker.export_chart_on_close) chart_ui.writeToDisk(&tracker);
	tracker.resetAllPlayerStats();
	writeIni();
	return 0;
}

/* window callback -- return is assigned to umsg (return zero to not be processed by arcdps or game) */
uintptr_t mod_wnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto const io = &ImGui::GetIO();

	switch (uMsg)
	{
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			const int vkey = (int)wParam;
			io->KeysDown[vkey] = 0;
			if (vkey == VK_CONTROL)
			{
				io->KeyCtrl = false;
			}
			else if (vkey == VK_MENU)
			{
				io->KeyAlt = false;
			}
			else if (vkey == VK_SHIFT)
			{
				io->KeyShift = false;
			}
			break;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			const int vkey = (int)wParam;
			io->KeysDown[vkey] = 1;
			if (vkey == VK_CONTROL)
			{
				io->KeyCtrl = true;
			}
			else if (vkey == VK_MENU)
			{
				io->KeyAlt = true;
			}
			else if (vkey == VK_SHIFT)
			{
				io->KeyShift = true;
			}
			break;
		}
		case WM_ACTIVATEAPP:
		{
			if (!wParam)
			{
				io->KeysDown[arc_global_mod1] = false;
				io->KeysDown[arc_global_mod2] = false;
			}
			break;
		}
	}

	if (io->KeysDown[arc_global_mod1] && io->KeysDown[arc_global_mod2])
	{
		if (io->KeysDown[log_key] || io->KeysDown[chart_key]) return 0;
	}

	return uMsg;
}

/* combat callback -- may be called asynchronously. return ignored */
/* one participant will be party/squad, or minion of. no spawn statechange events. despawn statechange only on marked boss npcs */
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision)
{
	PlayerEntry* current_entry = nullptr;

	/* ev is null. dst will only be valid on tracking add. skillname will also be null */
	if (!ev)
	{
		if (src)
		{
			/* notify tracking change */
			if (isPlayer(src) && !src->elite)
			{
				/* add */
				if (dst && src->prof)
				{
					tracker.addPlayer(src,dst);
				}

				/* remove */
				else
				{
					tracker.removePlayer(src);
				}
			}
		}
	}

	/* combat event. skillname may be null. non-null skillname will remain static until module is unloaded. refer to evtc notes for complete detail */
	else
	{

		/* common */

		/* statechange */
		if (ev->is_statechange)
		{
			switch(ev->is_statechange)
			{
			case CBTS_ENTERCOMBAT:
				tracker.processCombatEnter(ev, src);
				break;
			case CBTS_EXITCOMBAT:
				tracker.processCombatExit(ev, src);
				break;
			case CBTS_CHANGEUP:
				//TODO: make these into process functions in tracker.cpp
				if((current_entry = tracker.getPlayerEntry(src)))
				{
					current_entry->rally();
				}
				break;
			case CBTS_CHANGEDEAD:
				if((current_entry = tracker.getPlayerEntry(src)))
				{
					current_entry->dead();
				}
				break;
			case CBTS_CHANGEDOWN:
				if((current_entry = tracker.getPlayerEntry(src)))
				{
					current_entry->down();
				}
				break;
			case CBTS_LOGNPCUPDATE:
				tracker.processLogNpcUpdate(ev->src_agent);
				break;
			}
		}

		/* activation */
		else if (ev->is_activation)
		{

		}

		/* buff remove */
		else if (ev->is_buffremove)
		{
			if (ev->skillid==BUFF_STABILITY)//if it's stability
			{
				if(current_entry = tracker.getPlayerEntry(dst))
				{
					current_entry->setStabTime(ev->time+ms_per_tick);//cut the ending time of stab early
				}
			}
			else if (ev->skillid==BUFF_VAPOR_FORM//vapor form manual case
					 || ev->skillid==BUFF_ILLUSION_OF_LIFE//Illusion of Life manual case
					 )
			{
				if(current_entry = tracker.getPlayerEntry(dst))
				{
					current_entry->fixDoubleDown();
				}
			}

		}

		/* buff */
		else if (ev->buff)
		{
			if (ev->skillid==BUFF_STABILITY)//if it's stability
			{
				if(current_entry = tracker.getPlayerEntry(dst))
				{
					current_entry->setStabTime(ev->time+ev->value+ms_per_tick);//add prediction of when new stab will end
				}
			}
		}

		if(ev->result != CBTR_INTERRUPT && ev->result != CBTR_BLIND)
		{
			int64_t value = 0;
			current_entry = tracker.getPlayerEntry(src);
			PlayerEntry* other_entry = tracker.getPlayerEntry(dst);
			for(uint16_t index=0;index<getMechanics().size();index++)
			{
				if(value = getMechanics()[index].isValidHit(ev, src, dst,
					(current_entry ? current_entry->player : nullptr), //check for null before getting player object
					(other_entry ? other_entry->player: nullptr)))
				{
					tracker.processMechanic(ev, current_entry, other_entry, &getMechanics()[index], value);
					log_ui.scroll_to_bottom = true;
				}
			}
		}

		/* common */
	}

	return 0;
}

void ShowMechanicsLog(bool* p_open)
{
	if(show_app_log) log_ui.draw("Mechanics Log", p_open, ImGuiWindowFlags_NoCollapse
		| (!canMoveWindows() ? ImGuiWindowFlags_NoMove : 0), &tracker);
}

void ShowMechanicsChart(bool* p_open)
{
	if (show_app_chart)
	{
		chart_ui.draw(&tracker, "Mechanics Chart", p_open, ImGuiWindowFlags_NoCollapse
			| (!canMoveWindows() ? ImGuiWindowFlags_NoMove : 0), arc_clicklock_altui);
	}
}

uintptr_t mod_imgui(uint32_t not_charsel_or_loading)
{
	readArcExports();
	
	if (!not_charsel_or_loading) return 0;

	auto const io = &ImGui::GetIO();

	if (io->KeysDown[arc_global_mod1] && io->KeysDown[arc_global_mod2])
	{
		if (ImGui::IsKeyPressed(log_key))
		{
			show_app_log = !show_app_log;
		}
		if (ImGui::IsKeyPressed(chart_key))
		{
			show_app_chart = !show_app_chart;
		}
	}
	
	ShowMechanicsLog(&show_app_log);

	ShowMechanicsChart(&show_app_chart);

	return 0;
}

uintptr_t mod_options_end()
{
	options_ui.draw(&tracker);

	return 0;
}

uintptr_t mod_options_windows(const char* windowname)
{
	if (!windowname)
	{
		ImGui::Checkbox("Mechanics Log", &show_app_log);
		ImGui::Checkbox("Mechanics Chart", &show_app_chart);
	}

	return 0;
}

static int changeExportPath(const ImGuiInputTextCallbackData* data)
{
	chart_ui.export_dir = data->Buf;
}

void readArcExports()
{
	uint64_t e6_result = arc_export_e6();
	uint64_t e7_result = arc_export_e7();

	arc_hide_all = (e6_result & 0x01);
	arc_panel_always_draw = (e6_result & 0x02);
	arc_movelock_altui = (e6_result & 0x04);
	arc_clicklock_altui = (e6_result & 0x08);
	arc_window_fastclose = (e6_result & 0x10);


	uint16_t* ra = (uint16_t*)&e7_result;
	if (ra)
	{
		arc_global_mod1 = ra[0];
		arc_global_mod2 = ra[1];
		arc_global_mod_multi = ra[2];
	}
}

void parseIni()
{
	static bool has_read_ini = false;
	if (has_read_ini) return;

	SI_Error rc = mechanics_ini.LoadFile("addons\\arcdps\\arcdps_mechanics.ini");
	valid_mechanics_ini = rc >= 0;

	if (!valid_mechanics_ini) return;

	std::string pszValue = mechanics_ini.GetValue("log","show", "0");
	show_app_log = std::stoi(pszValue);

	pszValue = mechanics_ini.GetValue("chart", "show", "0");
	show_app_chart = std::stoi(pszValue);

	pszValue = mechanics_ini.GetValue("chart", "export_path", chart_ui.getDefaultExportPath().c_str());
	if (pszValue.length() < 5) pszValue = chart_ui.getDefaultExportPath();
	chart_ui.export_dir = pszValue;

	pszValue = mechanics_ini.GetValue("log", "key", "76");
	log_key = std::stoi(pszValue);

	pszValue = mechanics_ini.GetValue("chart", "key", "78");
	chart_key = std::stoi(pszValue);

	pszValue = mechanics_ini.GetValue("general", "self_only", std::to_string(tracker.show_only_self).c_str());
	tracker.show_only_self = std::stoi(pszValue);

	pszValue = mechanics_ini.GetValue("log", "max_mechanics", std::to_string(tracker.max_log_events).c_str());
	tracker.max_log_events = std::stoi(pszValue);

	pszValue = mechanics_ini.GetValue("chart", "export_on_close", "1");
	tracker.export_chart_on_close = std::stoi(pszValue);

	for (auto current_mechanic = getMechanics().begin(); current_mechanic != getMechanics().end(); ++current_mechanic)
	{
		pszValue = mechanics_ini.GetValue("mechanic verbosity",
			current_mechanic->getIniName().c_str(),
			std::to_string(current_mechanic->verbosity).c_str());
		
		current_mechanic->setVerbosity(std::stoi(pszValue));
	}
	has_read_ini = true;
}

void writeIni()
{
	SI_Error rc = mechanics_ini.SetValue("log", "show", std::to_string(show_app_log).c_str());
	rc = mechanics_ini.SetValue("chart", "show", std::to_string(show_app_chart).c_str());
	rc = mechanics_ini.SetValue("chart", "export_path", chart_ui.export_dir.c_str());

	rc = mechanics_ini.SetValue("log", "key", std::to_string(log_key).c_str());
	rc = mechanics_ini.SetValue("chart", "key", std::to_string(chart_key).c_str());
	
	rc = mechanics_ini.SetValue("general", "self_only", std::to_string(tracker.show_only_self).c_str());
	rc = mechanics_ini.SetValue("log", "max_mechanics", std::to_string(tracker.max_log_events).c_str());
	rc = mechanics_ini.SetValue("chart", "export_on_close", std::to_string(tracker.export_chart_on_close).c_str());

	for (auto current_mechanic = getMechanics().begin(); current_mechanic != getMechanics().end(); ++current_mechanic)
	{
		rc = mechanics_ini.SetValue("mechanic verbosity",
			current_mechanic->getIniName().c_str(),
			std::to_string(current_mechanic->verbosity).c_str());

	}

	rc = mechanics_ini.SaveFile("addons\\arcdps\\arcdps_mechanics.ini");
}

bool modsPressed()
{
	auto io = &ImGui::GetIO();

	return io->KeysDown[arc_global_mod1] && io->KeysDown[arc_global_mod2];
}

bool canMoveWindows()
{
	if (!arc_movelock_altui)
	{
		return true;
	}
	else
	{
		return modsPressed();
	}
}

bool canClickWindows()
{
	if (!arc_clicklock_altui)
	{
		return true;
	}
	else
	{
		return modsPressed();
	}
}