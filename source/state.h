#ifndef STATE_H
#define STATE_H

#include "header.h"

enum class Action;
class InputHandle;
class Board;
class Game;
class Texture;
class Renderer;
class Block;

class Button {
private:
	int pos_x, pos_y;
	int width, height;
	Texture* texture;
public:
	Button(string path, void (*callback) (), int posX = 0, int posY = 0);
	~Button();

	int get_pos_x();
	int get_pos_y();
	int get_width();
	int get_height();

	void draw();
	void (*callback_function) ();
};

/////////////////////////////////////////////////////////////////////

enum StateID { STATE_NULL, STATE_EXIT, STATE_POP, STATE_PLAY, STATE_MENU, STATE_PAUSE, STATE_GAMEOVER };

class State {
protected:
	InputHandle* input_handle;
	Renderer* renderer;
	
	Sounds* button_sound;
	Sounds* select_sound;
	Sounds* move_sound;
	Sounds* hard_drop_sound;
	Sounds* delete_line_sound;
	Sounds* option_change_sound;
	Sounds* rotate_sound;
	Sounds* gameover_sound;
public:
	StateID nextStateID;
	State(InputHandle* input_handle);

	virtual void init() = 0;
	virtual void exit() = 0;
	virtual void run() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

	void load_sounds();
	void pop_state(Game* game);
	void push_state(Game* game, State* state);
	void change_state(Game* game, State* state);
};

class GameState : public State {
public:
	enum GamePhase { GAME_STARTED, GAME_PLAYING, GAME_FINISHED };
	GameState(InputHandle* input_handle);
	~GameState();

	void init() override;
	void exit() override;
	void run() override;
	void update() override;
	void draw() override;
private:
	GamePhase current_phase;
	Board* board;
	Block current_block{ 0, 0 };
	Block next_block{ 0, 0 };
	Block hold_block{ 0, 0 };
	Block ghost_block{ 0, 0 };

	bool hold_block_first_time;
	bool hold_block_used;
	bool game_just_started;
	unsigned long long time_snap1;
	unsigned long long time_snap2;

	Texture* bg_game;
	Texture* countdown_texture;
	Texture* tetromino_sprite;
	SDL_Rect tetromino_sprite_clips[7];
	Texture* playfield_frame;
	SDL_Rect playfield_frame_clips[4];

	Score score;
	Texture* type_of_difficulty_text;
	Texture* number_high_score_text;
	Texture* number_scores_text;
	Texture* number_lines_text;

	bool is_game_over();
	void create_new_block();
	void check_state();
	void handle_event(Action action);
	void move_down_block();

	void draw_board();
	void draw_current_block(Block block);
	void draw_next_block(Block block);
	void draw_hold_block(Block block);
	void draw_ghost_block(Block block);
};

class MenuState : public State {
private:
	vector<Button*> buttons;
	Texture* bg_menu;
	int index;
public:
	MenuState(InputHandle* input_handle);
	~MenuState();

	void init() override;
	void exit() override;
	void run() override;
	void update() override;
	void draw() override;

	void add_button(Button* button);
};

class HelpState : public State {
private:
	Button* OK_button;
	Texture* bg_help;
public:
	HelpState(InputHandle* input_handle);
	~HelpState();

	void init() override;
	void exit() override;
	void run() override;
	void update() override;
	void draw() override;
};

class OptionState : public State {
	enum class SettingChange { left, right };
private:
	Button* OK_button;
	Texture* bg_option;
	Texture* difficulty_setting_text;
	Texture* difficulty_text;
	Texture* left_arrow;
	Texture* right_arrow;
	Texture* ghost_block_setting_text;
	Texture* music_sound_text;
	Texture* texture_on_on;
	Texture* texture_on_off;
	Texture* texture_off_on;
	Texture* texture_off_off;
	int index;

	void change_difficulty(SettingChange change);
	void change_ghost_block(SettingChange change);
	void change_music_sounds(SettingChange change);
public:
	OptionState(InputHandle* input_handle);
	~OptionState();

	void init() override;
	void exit() override;
	void run() override;
	void update() override;
	void draw() override;
};

class AboutState : public State {
private:
	Button* OK_button;
	Texture* bg_about;
public:
	AboutState(InputHandle* input_handle);
	~AboutState();

	void init() override;
	void exit() override;
	void run() override;
	void update() override;
	void draw() override;
};

class PauseState : public State {
private:
	vector<Button*> buttons;
	Texture* paused_frame;
	int index;
public:
	PauseState(InputHandle* input_handle);
	~PauseState();

	void init() override;
	void exit() override;
	void run() override;
	void update() override;
	void draw() override;
};

class GameOverState : public State {
private:
	vector<Button*> buttons;
	Score score;
	Texture* gameover_frame;
	Texture* highscore_gameover;
	Texture* score_gameover;
	int index;
public:
	GameOverState(InputHandle* input_handle);
	~GameOverState();

	void init() override;
	void exit() override;
	void run() override;
	void update() override;
	void draw() override;
};

#endif // !STATE_H