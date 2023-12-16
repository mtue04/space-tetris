#ifndef GAME_H
#define GAME_H

#include "header.h"

class InputHandle;
class Renderer;
class Sounds;
class State;
class GameState;
class MenuState;
class OptionState;
class HelpState;
class AboutState;
class PauseState;
class GameOverState;

class Game {
private:
	static Game* instance;
	SDL_Window* window;
	InputHandle* input_handle;
	vector<State*> states;

	Sounds* bg_music;
	GameState* game_state;
	MenuState* menu_state;
	OptionState* option_state;
	HelpState* help_state;
	AboutState* about_state;
	PauseState* pause_state;
	GameOverState* game_over_state;

	Game();
public:
	friend class OptionState;	// change window size
	Renderer* m_renderer;		// render all
	static Game* get_instance();
	
	bool init();
	void exit();
	void run();
	
	void pause_music(bool pause);
	void turn_music();

	void pop_state();
	void push_state(State* state);
	void change_state(State* state);

	static void push_options();
	static void push_help();
	static void push_about();
	static void push_new_game();
	static void push_paused();
	static void push_game_over();
	static void go_back();
	static void go_double_back();

	bool is_quit_game();
};

#endif