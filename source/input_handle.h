#ifndef INPUT_HANDLE_H
#define INPUT_HANDLE_H

#include "header.h"

enum class Action {
	stay_idle, back, move_up, move_down, move_left, move_right, select, drop, rotate, hold, pause
};

class InputHandle {
private:
	bool quit;
	Action action;
public:
	InputHandle();

	Action get_action();
	bool is_quit_game();
	void set_quit();

	void clear_event_queue();
	bool poll_action();
};

#endif // INPUT_HANDLE_H