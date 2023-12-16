#include "game.h"

Game::Game() {}

Game* Game::instance = 0;

Game* Game::get_instance() {
	if (instance == nullptr)
		instance = new Game;
	return instance;
}

bool Game::init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cerr << "Game: Could not initialize SDL! SDL_Error: " << SDL_GetError() << endl;
		return false;
	}
	
	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_window, height_window, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		cerr << "Game: Could not create window! SDL_Error: " << SDL_GetError() << endl;
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0 || IMG_Init(IMG_INIT_JPG) == 0) {
		cerr << "Game: Could not initialize SDL_image! SDL_image error: " << IMG_GetError() << endl;
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		cerr << "Game: Could not initialize SDL_mixer! SDL_mixer error: " << Mix_GetError() << endl;
		return false;
	}

	if (TTF_Init() == -1) {
		cerr << "Game: Could not initialize SDL_ttf! SDL_ttf error: " << TTF_GetError() << endl;
		return false;
	}

	m_renderer = new Renderer();
	m_renderer->init(window);
	SDL_RenderSetLogicalSize(m_renderer->renderer, width_window, height_window);
	SDL_SetWindowSize(window, width_window, height_window);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	bg_music = new Sounds();
	bg_music->load_music("assets/sound/music-bg.mp3");
	bg_music->play_music();

	input_handle = new InputHandle;

	menu_state = new MenuState(input_handle);
	menu_state->init();
	push_state(menu_state);
	
	return true;
}

void Game::exit() {
	for (const auto& it : states)
		delete it;
	delete m_renderer;
	delete bg_music;

	SDL_DestroyWindow(window);
	window = nullptr;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Game::run() {
	if (!states.empty())
		states.back()->run();
}

void Game::pause_music(bool pause) {
	bg_music->pause_music(pause);
}

void Game::turn_music() {
	bg_music->play_music();
}

void Game::pop_state() {
	states.pop_back();
}

void Game::push_state(State* state) {
	states.push_back(state);
}

void Game::change_state(State* state) {
	pop_state();
	push_state(state);
}

void Game::push_new_game() {
	delete Game::get_instance()->game_state;
	Game::get_instance()->bg_music->pause_music(true);
	Game::get_instance()->game_state = new GameState(Game::get_instance()->input_handle);
	Game::get_instance()->game_state->init();
	Game::get_instance()->push_state(Game::get_instance()->game_state);
}

void Game::push_options() {
	delete Game::get_instance()->option_state;
	Game::get_instance()->option_state = new OptionState(Game::get_instance()->input_handle);
	Game::get_instance()->option_state->init();
	Game::get_instance()->push_state(Game::get_instance()->option_state);
}

void Game::push_help() {
	delete Game::get_instance()->help_state;
	Game::get_instance()->help_state = new HelpState(Game::get_instance()->input_handle);
	Game::get_instance()->help_state->init();
	Game::get_instance()->push_state(Game::get_instance()->help_state);
}

void Game::push_about() {
	delete Game::get_instance()->about_state;
	Game::get_instance()->about_state = new AboutState(Game::get_instance()->input_handle);
	Game::get_instance()->about_state->init();
	Game::get_instance()->push_state(Game::get_instance()->about_state);
}

void Game::push_paused() {
	delete Game::get_instance()->pause_state;
	Game::get_instance()->pause_state = new PauseState(Game::get_instance()->input_handle);
	Game::get_instance()->pause_state->init();
	Game::get_instance()->push_state(Game::get_instance()->pause_state);
}

void Game::push_game_over() {
	delete Game::get_instance()->game_over_state;
	Game::get_instance()->game_over_state = new GameOverState(Game::get_instance()->input_handle);
	Game::get_instance()->game_over_state->init();
	Game::get_instance()->push_state(Game::get_instance()->game_over_state);
}

void Game::go_back() {
	Game::get_instance()->pop_state();
}

void Game::go_double_back() {
	Game::get_instance()->pop_state();
	Game::get_instance()->pop_state();
}

bool Game::is_quit_game() {
	if (states.empty())
		return true;
	return states.back()->nextStateID == STATE_EXIT;
}