#pragma once

#include "mechanics.h"
#include "player.h"

class LogEvent
{
public:

	Player* player = nullptr;
	Mechanic* mechanic = nullptr;
	int time = 0;//time from starting the pull to the event happening
	uint64_t time_absolute = 0;//ev->time at the instance of event
	std::string str = "";
	int64_t value = 1;
	cbtevent ev;

	LogEvent(Player* new_player, Mechanic* new_mechanic, uint64_t new_time, uint64_t new_time_absolute, int64_t new_value, const cbtevent* new_ev);

	void draw();
	void drawTooltip();
	void bakeStr();
	bool isPlaceholder();
	std::string getFilterText();
};

