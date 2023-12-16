#include "state.h"

Button::Button(string path, void (*callback) (), int posX, int posY) {
    callback_function = callback;
    pos_x = posX;
    pos_y = posY;
    texture = new Texture();
    texture->load_image(path);
    width = texture->get_width();
    height = texture->get_height();
}

Button::~Button() {
    texture->free();
}

int Button::get_pos_x() { return pos_x; }

int Button::get_pos_y() { return pos_y; }

int Button::get_width() { return width; }

int Button::get_height() { return height; }

void Button::draw() {
    texture->render(pos_x, pos_y);
}

/////////////////////////////////////////////////////////////////////

State::State(InputHandle* input_handle) : input_handle(input_handle) {
    load_sounds();
}

void State::pop_state(Game* game) {
    game->pop_state();
}

void State::push_state(Game* game, State* state) {
    game->push_state(state);
}

void State::change_state(Game* game, State* state) {
    game->change_state(state);
}

void State::load_sounds() {
    button_sound = new Sounds();
    button_sound->load_sound("assets/sound/button.wav");
    select_sound = new Sounds();
    select_sound->load_sound("assets/sound/select.wav");
    move_sound = new Sounds();
    move_sound->load_sound("assets/sound/move.wav");
    hard_drop_sound = new Sounds();
    hard_drop_sound->load_sound("assets/sound/hard-drop.wav");
    delete_line_sound = new Sounds();
    delete_line_sound->load_sound("assets/sound/delete-line.wav");
    option_change_sound = new Sounds();
    option_change_sound->load_sound("assets/sound/option-change.wav");
    rotate_sound = new Sounds();
    rotate_sound->load_sound("assets/sound/rotate.wav");
    gameover_sound = new Sounds();
    gameover_sound->load_sound("assets/sound/gameover.wav");
}

/////////////////////////////////////////////////////////////////////

GameState::GameState(InputHandle* input_handle) : State(input_handle) {}

GameState::~GameState() {
    exit();
}

void GameState::init() {
    current_phase = GAME_STARTED;
    board = new Board();
    srand(time(nullptr));
    hold_block_first_time = true;
    hold_block_used = false;

    // Get the first block
    next_block.set_type(rand() % 7);
    next_block.set_rotation(0);
    create_new_block();
    next_block.set_row(next_box_y);
    next_block.set_col(next_box_x);

    // Load textures
    bg_game = new Texture();
    bg_game->load_image("assets/bg/bg-game.png");
    countdown_texture = new Texture();
    tetromino_sprite = new Texture();
    playfield_frame = new Texture();
    tetromino_sprite->load_image("assets/button/tetromino-sprites.png");
    playfield_frame->load_image("assets/button/playfield-frame.png");

    if (tetromino_sprite == nullptr || playfield_frame == nullptr)
        cerr << "State: Could not load image." << endl;
    // Create sprite clips
    for (int i = 0; i < 7; i++) {
        tetromino_sprite_clips[i].x = i * block_size;
        tetromino_sprite_clips[i].y = 0;
        tetromino_sprite_clips[i].w = block_size;
        tetromino_sprite_clips[i].h = block_size;
    }

    for (int i = 0; i < 4; i++) {
        playfield_frame_clips[i].x = i * frame_sprite_size;
        playfield_frame_clips[i].y = 0;
        playfield_frame_clips[i].w = frame_sprite_size;
        playfield_frame_clips[i].h = frame_sprite_size;
    }

    // Load information
    type_of_difficulty_text = new Texture();
    string type_difficulty;
    if (difficulty_index == 0)
        type_difficulty = "NEWBIE";
    else if (difficulty_index == 1)
        type_difficulty = "EASY";
    else if (difficulty_index == 2)
        type_difficulty = "NORMAL";
    else if (difficulty_index == 3)
        type_difficulty = "HARD";
    type_of_difficulty_text->load_text(type_difficulty, Game::get_instance()->m_renderer->small_font, default_text_color);
    number_scores_text = new Texture();
    number_lines_text = new Texture();

    game_just_started = true;
}

void GameState::exit() {
    delete bg_game;
    delete board;
    delete countdown_texture;
    delete tetromino_sprite;
    delete playfield_frame;
    delete type_of_difficulty_text;
    delete number_scores_text;
    delete number_lines_text;
}

void GameState::run() {
    switch (current_phase) {
    case GAME_STARTED:
    {
        if (game_just_started) {
            time_snap1 = SDL_GetTicks();
            game_just_started = false;
        }
        unsigned long long ms_passed = SDL_GetTicks() - time_snap1;
        if (ms_passed < 3000) {
            while (input_handle->poll_action()) {
                if (input_handle->is_quit_game()) {
                    nextStateID = STATE_EXIT;
                    break;
                }
                if (input_handle->get_action() == Action::back) {
                    Game::get_instance()->pop_state();
                    break;                                                      // Pop the state only once even if Action::back is pressed twice
                }
            }
            Game::get_instance()->m_renderer->clear_screen();
            draw();
            int countdown_time = ceil((3000 - ms_passed) / 1000.0);             // The time left on the countdown
            if (countdown_time >= 0) {
                countdown_texture->load_text(to_string(countdown_time), Game::get_instance()->m_renderer->medium_font, default_text_color);
                Game::get_instance()->m_renderer->render_texture(countdown_texture, width_window / 2, height_window / 2);
            }
            Game::get_instance()->m_renderer->update_screen();
        } else {
            current_phase = GAME_PLAYING;
            time_snap1 = SDL_GetTicks();
        }
        break;
    }
    case GAME_PLAYING:
        if (input_handle->is_quit_game()) {
            nextStateID = STATE_EXIT;
        } else if (!is_game_over()) {
            while (input_handle->poll_action()) {
                if (input_handle->get_action() == Action::back) {
                    Game::get_instance()->pop_state();
                    break;                                                // Pop the state only once even if Action::back is pressed twice
                } else {
                    handle_event(input_handle->get_action());
                }
            }

            time_snap2 = SDL_GetTicks();
            if (time_snap2 - time_snap1 >= wait_time) {
                move_down_block();
                time_snap1 = SDL_GetTicks();
            }
            Game::get_instance()->m_renderer->clear_screen();
            draw();
            Game::get_instance()->m_renderer->update_screen();
        } else {
            // Here the game has finished
            current_phase = GAME_FINISHED;
            your_score = score.get_score();
            gameover_sound->play_sound();
            score.save_score();
        }
        break;

    case GAME_FINISHED:
        if (!input_handle->is_quit_game()) {
            while (input_handle->poll_action() != 0) {
                if (input_handle->get_action() == Action::back) {
                    Game::get_instance()->pop_state();
                    break;                                          // Pop the state only once even if Action::back is pressed twice
                }
            }
            Game::get_instance()->m_renderer->clear_screen();
            draw();
            Game::get_instance()->pop_state();
            Game::push_game_over();
            //Game::get_instance()->m_renderer->update_screen();
        } else {
            nextStateID = STATE_EXIT;
        }
        break;
    }
}

void GameState::update() {}

void GameState::draw() {
    bg_game->render(0, 0);
    draw_board();
    draw_current_block(current_block);
    if (!board->is_game_over() && ghost_block_enabled) 
        draw_ghost_block(current_block);
    if (!hold_block_first_time) draw_hold_block(hold_block);
        draw_next_block(next_block);
    
    type_of_difficulty_text->render_center(840, 375);
    number_scores_text->load_text(to_string(score.get_score()), Game::get_instance()->m_renderer->small_font, default_text_color);
    number_scores_text->render_center(840, 415);
    number_lines_text->load_text(to_string(score.get_lines()), Game::get_instance()->m_renderer->small_font, default_text_color);
    number_lines_text->render_center(840, 455);
    
}

bool GameState::is_game_over() {
    return board->is_game_over();
}

void GameState::create_new_block() {
    current_block.set_type(next_block.get_type());
    current_block.set_rotation(next_block.get_rotation());
    current_block.set_row(current_block.get_init_row_offset());
    current_block.set_col(playfield_width / 2 + current_block.get_init_col_offset());

    for (int i = 0; i < 2; i++) {
        current_block.set_row(current_block.get_row() + 1);
        if (!board->is_legal_position(current_block))
            current_block.set_row(current_block.get_row() - 1);
    }
    
    if (current_block.get_type() > 1) {
        current_block.set_row(current_block.get_row() + 1);
        if (!board->is_legal_position(current_block))
            current_block.set_row(current_block.get_row() - 1);
    }

    next_block.set_type(rand() % 7);
    next_block.set_rotation(0);
}

void GameState::check_state() {
    board->store_block(current_block);
    int num_deleted_rows = board->delete_full_rows();
    if (num_deleted_rows != 0) {
        delete_line_sound->play_sound();
        score.update_score(score.get_score() + num_deleted_rows * score_per_line * difficulty_index);
        score.update_lines(score.get_lines() + num_deleted_rows);
    }
    if (!board->is_game_over()) {
        create_new_block();
    }
    // Can hold block again
    hold_block_used = false;
}

void GameState::handle_event(Action action) {
    switch (action) {
    case Action::move_down:
        move_sound->play_sound();
        current_block.set_row(current_block.get_row() + 1);
        if (!board->is_legal_position(current_block)) {
            current_block.set_row(current_block.get_row() - 1);
            check_state();
        }
        break;
    case Action::move_left:
        move_sound->play_sound();
        current_block.set_col(current_block.get_col() - 1);
        if (!board->is_legal_position(current_block))
            current_block.set_col(current_block.get_col() + 1);
        break;
    case Action::move_right:
        move_sound->play_sound();
        current_block.set_col(current_block.get_col() + 1);
        if (!board->is_legal_position(current_block))
            current_block.set_col(current_block.get_col() - 1);
        break;
    case Action::drop:
        hard_drop_sound->play_sound();
        while (board->is_legal_position(current_block))
            current_block.set_row(current_block.get_row() + 1);
        current_block.set_row(current_block.get_row() - 1);
        check_state();
        break;
    case Action::move_up:
        rotate_sound->play_sound();
        current_block.set_rotation((current_block.get_rotation() + 1) % 4);
        if (!board->is_legal_position(current_block))
            current_block.set_rotation((current_block.get_rotation() + 3) % 4);
        break;
    case Action::rotate:
        rotate_sound->play_sound();
        current_block.set_rotation((current_block.get_rotation() + 3) % 4);
        if (!board->is_legal_position(current_block))
            current_block.set_rotation((current_block.get_rotation() + 1) % 4);
        break;
    case Action::hold:
        if (hold_block_first_time) {
            hold_block = Block(current_block);
            hold_block.set_rotation(0);
            create_new_block();
            hold_block_first_time = false;
            hold_block_used = true;
        } else if (!hold_block_used) {
            swap(current_block, hold_block);
            hold_block.set_rotation(0);
            current_block.set_row(current_block.get_init_row_offset());
            current_block.set_col(playfield_width / 2 + current_block.get_init_col_offset());

            for (int i = 0; i < 2; i++) {
                current_block.set_row(current_block.get_row() + 1);
                if (!board->is_legal_position(current_block))
                    current_block.set_row(current_block.get_row() - 1);
            }

            if (current_block.get_type() > 1) {
                current_block.set_row(current_block.get_row() + 1);
                if (!board->is_legal_position(current_block))
                    current_block.set_row(current_block.get_row() - 1);
            }

            hold_block_used = true;
        }
        break;
    case Action::pause:
        current_phase = GAME_STARTED;
        game_just_started = true;
        Game::get_instance()->push_paused();
        break;
    }
}

void GameState::move_down_block() {
    current_block.set_row(current_block.get_row() + 1);
    if (!board->is_legal_position(current_block)) {
        current_block.set_row(current_block.get_row() - 1);
        check_state();
    }
}

void GameState::draw_board() {
    for (int i = 0; i < 2 * true_playfield_height; i++) {
        // Left frame
        playfield_frame->render(width_to_playfield - frame_sprite_size, height_to_playfield + i * frame_sprite_size, &playfield_frame_clips[0]);
        // Right frame
        playfield_frame->render(width_to_playfield + block_size * playfield_width - (frame_sprite_size - frame_width), height_to_playfield + i * frame_sprite_size, &playfield_frame_clips[0]);
    }

    // 2 corners
    playfield_frame->render(width_to_playfield - frame_sprite_size, height_to_playfield + block_size * true_playfield_height - (frame_sprite_size - frame_width), &playfield_frame_clips[2]);
    playfield_frame->render(width_to_playfield + block_size * playfield_width, height_to_playfield + block_size * true_playfield_height - (frame_sprite_size - frame_width), &playfield_frame_clips[3]);

    // Bottom frame
    for (int i = 0; i < 2 * playfield_width; i++) {
        playfield_frame->render(width_to_playfield + i * frame_sprite_size, height_to_playfield + block_size * true_playfield_height, &playfield_frame_clips[1]);
    }

    // Draw placed blocks
    for (int row = 0; row < playfield_height; row++)
        for (int col = 0; col < playfield_width; col++)
            if (!board->is_empty_block(row, col))
                tetromino_sprite->render(width_to_playfield + col * block_size, height_to_playfield + (row - (playfield_height - true_playfield_height)) * block_size, &tetromino_sprite_clips[board->get_tetromino (row, col)]);
}

void GameState::draw_current_block(Block block) {
    for (int row = 0; row < matrix_blocks; row++)
        for (int col = 0; col < matrix_blocks; col++)
            if (block.get_type(row, col) != 0)
                tetromino_sprite->render(width_to_playfield + (col + block.get_col()) * block_size, height_to_playfield + (row + block.get_row() - (playfield_height - true_playfield_height)) * block_size, &tetromino_sprite_clips[block.get_type()]);
}

void GameState::draw_next_block(Block block) {
    int next_block_x = 798 - (matrix_blocks * block_size) / 2;
    int next_block_y = 190 - (matrix_blocks * block_size) / 2;
    if (next_block.get_type() == 0) {
        next_block_x -= 10;
        next_block_y -= 20;
    } else if (next_block.get_type() == 1) {
        next_block_x -= 12;
    }

    for (int row = 0; row < matrix_blocks; row++)
        for (int col = 0; col < matrix_blocks; col++)
            if (block.get_type(row, col) != 0)
                tetromino_sprite->render(next_block_x + col * block_size, next_block_y + row * block_size, &tetromino_sprite_clips[block.get_type()]);
}

void GameState::draw_hold_block(Block block) {
    int hold_block_x = 798 - (matrix_blocks * block_size) / 2;
    int hold_block_y = 317 - (matrix_blocks * block_size) / 2;
    if (hold_block.get_type() == 0) {
        hold_block_x -= 10;
        hold_block_y -= 20;
    } else if (hold_block.get_type() == 1) {
        hold_block_x -= 12;
    }

    for (int row = 0; row < matrix_blocks; row++)
        for (int col = 0; col < matrix_blocks; col++)
            if (block.get_type(row, col) != 0)
                tetromino_sprite->render(hold_block_x + col * block_size, hold_block_y + row * block_size, &tetromino_sprite_clips[block.get_type()]);
}

void GameState::draw_ghost_block(Block block) {
    ghost_block = block;
    while (board->is_legal_position(ghost_block))
        ghost_block.set_row(ghost_block.get_row() + 1);
    ghost_block.set_row(ghost_block.get_row() - 1);

    // Change transparency to render ghost block
    tetromino_sprite->set_alpha_mode(transparency_alpha);

    for (int row = 0; row < matrix_blocks; row++)
        for (int col = 0; col < matrix_blocks; col++)
            if (ghost_block.get_type(row, col) != 0)
                tetromino_sprite->render(width_to_playfield + (col + ghost_block.get_col()) * block_size, height_to_playfield + (row + ghost_block.get_row() - (playfield_height - true_playfield_height)) * block_size, &tetromino_sprite_clips[ghost_block.get_type()]);

    // Change to normal
    tetromino_sprite->set_alpha_mode(255);
}

/////////////////////////////////////////////////////////////////////

MenuState::MenuState(InputHandle* input_handle) : State(input_handle) {}

MenuState::~MenuState() {
    exit();
}

void MenuState::init() {
    index = 0;
    bg_menu = new Texture();
    bg_menu->load_image("assets/bg/bg-menu.png");
    if (bg_menu == nullptr)
        cerr << "State: Could not load image. SDL_Error: " << SDL_GetError() << endl;

    buttons.push_back(new Button("assets/button/button-play.png", &Game::push_new_game, (width_window - 200) / 2, 180));
    buttons.push_back(new Button("assets/button/button-options.png", &Game::push_options, (width_window - 200) / 2, 250));
    buttons.push_back(new Button("assets/button/button-help.png", &Game::push_help, (width_window - 200) / 2, 320));
    buttons.push_back(new Button("assets/button/button-about.png", &Game::push_about, (width_window - 200) / 2, 390));
    buttons.push_back(new Button("assets/button/button-exit.png", &Game::go_back, (width_window - 200) / 2, 460));
}

void MenuState::exit() {
    for (const auto& it : buttons)
        delete it;
}

void MenuState::run() {
    update();
    draw();
}

void MenuState::update() {
    Game::get_instance()->pause_music(false);
    while (input_handle->poll_action() != 0) {
        if (input_handle->is_quit_game()) {
            nextStateID = STATE_EXIT;
            break;
        }
        switch (input_handle->get_action()) {
        case Action::move_up:
            if (index > 0) {
                button_sound->play_sound();
                --index;
            }
            break;
        case Action::move_down:
            if (index < buttons.size() - 1) {
                button_sound->play_sound();
                ++index;
            }
            break;
        case Action::select:
            select_sound->play_sound();
            buttons[index]->callback_function();
            break;
        default:
            break;
        }
    }
}

void MenuState::draw() {
    Game::get_instance()->m_renderer->clear_screen();
    bg_menu->render(0, 0);
    for (const auto& it : buttons)
        it->draw();

    SDL_Rect highlight_box = { buttons[index]->get_pos_x(), buttons[index]->get_pos_y(), buttons[index]->get_width(), buttons[index]->get_height() };
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game::get_instance()->m_renderer->renderer, 255, 255, 255,transparency_alpha - 20);
    SDL_RenderFillRect(Game::get_instance()->m_renderer->renderer, &highlight_box);
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_NONE);

    Game::get_instance()->m_renderer->update_screen();
}

void MenuState::add_button(Button* button) {
    buttons.push_back(button);
}

/////////////////////////////////////////////////////////////////////

OptionState::OptionState(InputHandle* input_handle) : State(input_handle) {}

OptionState::~OptionState() {
    exit();
}

void OptionState::init() {
    index = 0;

    bg_option = new Texture();
    bg_option->load_image("assets/bg/bg-options.png");
    difficulty_setting_text = new Texture();
    difficulty_setting_text->load_text("Difficulty", Game::get_instance()->m_renderer->medium_font, default_text_color);
    difficulty_text = new Texture();
    ghost_block_setting_text = new Texture();
    ghost_block_setting_text->load_text("Ghost Block", Game::get_instance()->m_renderer->medium_font, default_text_color);
    music_sound_text = new Texture();
    music_sound_text->load_text("Music / Sounds", Game::get_instance()->m_renderer->medium_font, default_text_color);

    left_arrow = new Texture();
    right_arrow = new Texture();
    texture_on_on = new Texture(); 
    texture_on_off = new Texture();
    texture_off_on = new Texture();
    texture_off_off = new Texture();

    left_arrow->load_image("assets/button/arrow-left.png");
    right_arrow->load_image("assets/button/arrow-right.png");
    texture_on_on->load_image("assets/button/button-on-on.png");
    texture_on_off->load_image("assets/button/button-on-off.png");
    texture_off_on->load_image("assets/button/button-off-on.png");
    texture_off_off->load_image("assets/button/button-off-off.png");
    OK_button = new Button("assets/button/button-ok.png", &Game::go_back, (width_window - 200) / 2, 450);

    if (left_arrow == nullptr || right_arrow == nullptr || texture_on_on == nullptr || texture_on_off == nullptr || texture_off_on == nullptr || texture_off_off == nullptr || OK_button == nullptr)
        cerr << "State: Could not load image." << endl;
}

void OptionState::exit() {
    delete bg_option;
    delete difficulty_setting_text;
    delete difficulty_text;
    delete ghost_block_setting_text;
    delete music_sound_text;
    delete texture_on_on;
    delete texture_on_off;
    delete texture_off_on;
    delete texture_off_off;
    delete OK_button;
}

void OptionState::run() {
    update();
    draw();
}

void OptionState::update() {
    while (input_handle->poll_action() != 0) {
        if (input_handle->is_quit_game()) {
            nextStateID = STATE_EXIT;
            break;
        }

        switch (input_handle->get_action()) {
        case Action::move_up:
            if (index > 0) {
                index--;
                button_sound->play_sound();
            }
            break;
        case Action::move_down:
            if (index < 3) {
                ++index;
                button_sound->play_sound();
            }
            break;
        case Action::move_left:
            if (index == 0) {
                option_change_sound->play_sound();
                change_difficulty(SettingChange::left);
            } else if (index == 1) {
                option_change_sound->play_sound();
                change_ghost_block(SettingChange::left);
            } else if (index == 2) {
                option_change_sound->play_sound();
                change_music_sounds(SettingChange::left);
            }
            break;
        case Action::move_right:
            if (index == 0) {
                option_change_sound->play_sound();
                change_difficulty(SettingChange::right);
            } else if (index == 1) {
                option_change_sound->play_sound();
                change_ghost_block(SettingChange::right);
            } else if (index == 2) {
                option_change_sound->play_sound();
                change_music_sounds(SettingChange::right);
            }
            break;
        case Action::back:
            Game::get_instance()->pop_state();
            break;
        case Action::select:
            if (index == 3) {
                select_sound->play_sound();
                Game::get_instance()->pop_state();
            }
            break;
        default:
            break;
        }
    }
}

void OptionState::draw() {
    Game::get_instance()->m_renderer->clear_screen();

    bg_option->render(0, 0);
    difficulty_setting_text->render(60, 200);
    ghost_block_setting_text->render(60, 280);
    music_sound_text->render(60, 360);

    string difficulty_string;
    if (difficulty_index == 0)
        difficulty_string = "NEWBIE";
    else if (difficulty_index == 1)
        difficulty_string = "EASY";
    else if (difficulty_index == 2)
        difficulty_string = "NORMAL";
    else if (difficulty_index == 3)
        difficulty_string = "HARD";
    difficulty_text->load_text(difficulty_string, Game::get_instance()->m_renderer->medium_font, default_text_color);
    difficulty_text->render(550 + (200 - difficulty_text->get_width()) / 2, 200);
    
    left_arrow->render(480, 195);
    right_arrow->render(750, 195);

    if (ghost_block_enabled) {
        texture_off_off->render(400, 280);
        texture_on_on->render(680, 280);
    }
    else {
        texture_off_on->render(400, 280);
        texture_on_off->render(680, 280);
    }

    if (is_music_sounds_on) {
        texture_off_off->render(400, 360);
        texture_on_on->render(680, 360);
    } else {
        texture_off_on->render(400, 360);
        texture_on_off->render(680, 360);
    }

    OK_button->draw();
    if (index <= 2) {
        int height = difficulty_setting_text->get_height();
        SDL_Rect highlight_box = { 0, 200 + (index * 80), width_window, height + 5 };
        SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(Game::get_instance()->m_renderer->renderer, 255, 255, 255, transparency_alpha - 20);
        SDL_RenderFillRect(Game::get_instance()->m_renderer->renderer, &highlight_box);
        SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_NONE);
    } else {
        SDL_Rect highlight_box = { OK_button->get_pos_x(), OK_button->get_pos_y(), OK_button->get_width(), OK_button->get_height() };
        SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(Game::get_instance()->m_renderer->renderer, 255, 255, 255, transparency_alpha - 20);
        SDL_RenderFillRect(Game::get_instance()->m_renderer->renderer, &highlight_box);
        SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_NONE);
    }

    Game::get_instance()->m_renderer->update_screen();
}

void OptionState::change_difficulty(SettingChange change) {
    if (change == SettingChange::left && difficulty_index > 0) {
        --difficulty_index;

        wait_time = wait_time_list[difficulty_index];
    } else if (change == SettingChange::right && difficulty_index < 3) {
        ++difficulty_index;
        wait_time = wait_time_list[difficulty_index];
    }
}

void OptionState::change_ghost_block(SettingChange change) {
    if ((change == SettingChange::left && ghost_block_enabled) || (change == SettingChange::right && !ghost_block_enabled))
        ghost_block_enabled = !ghost_block_enabled;
}

void OptionState::change_music_sounds(SettingChange change) {
    if ((change == SettingChange::left && is_music_sounds_on) || (change == SettingChange::right && !is_music_sounds_on)) {
        is_music_sounds_on = !is_music_sounds_on;
        Game::get_instance()->turn_music();
    }
}

/////////////////////////////////////////////////////////////////////

HelpState::HelpState(InputHandle* input_handle) : State(input_handle) {}

HelpState::~HelpState() {
    exit();
}

void HelpState::init() {
    bg_help = new Texture();
    bg_help->load_image("assets/bg/bg-help.png");
    OK_button = new Button("assets/button/button-ok.png", &Game::go_back, (width_window - 200) / 2, 450);
}

void HelpState::exit() {
    delete bg_help;
    delete OK_button;
}

void HelpState::run() {
    update();
    draw();
}

void HelpState::update() {
    while (input_handle->poll_action() != 0) {
        if (input_handle->is_quit_game()) {
            nextStateID = STATE_EXIT;
            break;
        }

        switch (input_handle->get_action()) {
        case Action::back:
            Game::get_instance()->pop_state();
            break;
        case Action::select:
            select_sound->play_sound();
            OK_button->callback_function();
            break;
        default:
            break;
        }
    }
}

void HelpState::draw() {
    Game::get_instance()->m_renderer->clear_screen();

    bg_help->render(0, 0);

    OK_button->draw();
    SDL_Rect highlight_box = { OK_button->get_pos_x(), OK_button->get_pos_y(), OK_button->get_width(), OK_button->get_height() };
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game::get_instance()->m_renderer->renderer, 255, 255, 255, transparency_alpha - 20);
    SDL_RenderFillRect(Game::get_instance()->m_renderer->renderer, &highlight_box);
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_NONE);
    Game::get_instance()->m_renderer->update_screen();
}

/////////////////////////////////////////////////////////////////////

AboutState::AboutState(InputHandle* input_handle) : State(input_handle) {}

AboutState::~AboutState() {
    exit();
}

void AboutState::init() {
    bg_about = new Texture();
    bg_about->load_image("assets/bg/bg-about.png");
    OK_button = new Button("assets/button/button-ok.png", &Game::go_back, (width_window - 200) / 2, 450);
}

void AboutState::exit() {
    delete OK_button;
    delete bg_about;
}

void AboutState::run() {
    update();
    draw();
}

void AboutState::update() {
    while (input_handle->poll_action() != 0) {
        if (input_handle->is_quit_game()) {
            nextStateID = STATE_EXIT;
            break;
        }

        switch (input_handle->get_action()) {
        case Action::back:
            Game::get_instance()->pop_state();
            break;
        case Action::select:
            select_sound->play_sound();
            OK_button->callback_function();
            break;
        default:
            break;
        }
    }
}

void AboutState::draw() {
    Game::get_instance()->m_renderer->clear_screen();

    bg_about->render(0, 0);

    OK_button->draw();
    SDL_Rect highlight_box = { OK_button->get_pos_x(), OK_button->get_pos_y(), OK_button->get_width(), OK_button->get_height() };
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game::get_instance()->m_renderer->renderer, 255, 255, 255, transparency_alpha - 20);
    SDL_RenderFillRect(Game::get_instance()->m_renderer->renderer, &highlight_box);
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_NONE);
    Game::get_instance()->m_renderer->update_screen();
}

/////////////////////////////////////////////////////////////////////

PauseState::PauseState(InputHandle* input_handle) : State(input_handle) {}

PauseState::~PauseState() {
    exit();
}

void PauseState::init() {
    index = 1;
    paused_frame = new Texture();
    paused_frame->load_image("assets/button/paused-frame.png");
    if (paused_frame == nullptr)
        cerr << "State: Could not load the frame from: assets/button/paused-frame.png" << endl;

    buttons.push_back(new Button("assets/button/button-quit.png", &Game::get_instance()->go_double_back, (width_window / 2) - 180, 280));
    buttons.push_back(new Button("assets/button/button-resume.png", &Game::get_instance()->go_back, (width_window / 2) + 30, 280));
}

void PauseState::exit() {
    for (const auto& it : buttons)
        delete it;
    delete paused_frame;
}

void PauseState::run() {
    update();
    draw();
}

void PauseState::update() {
    while (input_handle->poll_action() != 0) {
        if (input_handle->is_quit_game()) {
            nextStateID = STATE_EXIT;
            break;
        }

        switch (input_handle->get_action()) {
        case Action::move_left:
            if (index > 0) {
                option_change_sound->play_sound();
                --index;
            }
            break;
        case Action::move_right:
            if (index < buttons.size() - 1) {
                option_change_sound->play_sound();
                ++index;
            }
            break;
        case Action::select:
            select_sound->play_sound();
            buttons[index]->callback_function();
            break;
        default:
            break;
        }
    }
}

void PauseState::draw() {
    paused_frame->render_center(width_window / 2, height_window / 2);
    for (const auto &it : buttons)
        it->draw();

    SDL_Rect highlight_box = { buttons[index]->get_pos_x(), buttons[index]->get_pos_y(), buttons[index]->get_width(), buttons[index]->get_height() };
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game::get_instance()->m_renderer->renderer, 255, 255, 255, transparency_alpha - 20);
    SDL_RenderFillRect(Game::get_instance()->m_renderer->renderer, &highlight_box);
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_NONE);
    Game::get_instance()->m_renderer->update_screen();
}

/////////////////////////////////////////////////////////////////////

GameOverState::GameOverState(InputHandle* input_handle) : State(input_handle) {}

GameOverState::~GameOverState() {
    exit();
}

void GameOverState::init() {
    index = 1;

    gameover_frame = new Texture();
    gameover_frame->load_image("assets/button/gameover-frame.png");
    if (gameover_frame == nullptr)
        cerr << "State: Could not load the frame from: assets/button/gameover-frame.png" << endl;
    
    highscore_gameover = new Texture();
    highscore_gameover->load_text(to_string(score.get_high_score()), Game::get_instance()->m_renderer->medium_font, red_text_color);
    score_gameover = new Texture();
    score_gameover->load_text(to_string(your_score), Game::get_instance()->m_renderer->medium_font, red_text_color);
    your_score = 0;

    buttons.push_back(new Button("assets/button/button-quit.png", &Game::get_instance()->go_back, (width_window / 2) - 180, 330));
    buttons.push_back(new Button("assets/button/button-replay.png", &Game::get_instance()->push_new_game, (width_window / 2) + 30, 330));
}

void GameOverState::exit() {
    for (const auto& it : buttons)
        delete it;
    delete gameover_frame;
}

void GameOverState::run() {
    update();
    draw();
}

void GameOverState::update() {
    while (input_handle->poll_action() != 0) {
        if (input_handle->is_quit_game()) {
            nextStateID = STATE_EXIT;
            break;
        }

        switch (input_handle->get_action()) {
        case Action::move_left:
            if (index > 0) {
                option_change_sound->play_sound();
                --index;
            }
            break;
        case Action::move_right:
            if (index < buttons.size() - 1) {
                option_change_sound->play_sound();
                ++index;
            }
            break;
        case Action::select:
            select_sound->play_sound();
            if (index == 1) {
                Game::get_instance()->go_back();
            }
            buttons[index]->callback_function();
            break;
        default:
            break;
        }
    }
}

void GameOverState::draw() {
    gameover_frame->render_center(width_window / 2, height_window / 2);
    for (const auto& it : buttons)
        it->draw();

    highscore_gameover->render_center((width_window / 2) + 110, 242);
    score_gameover->render_center((width_window / 2) + 110, 293);

    SDL_Rect highlight_box = { buttons[index]->get_pos_x(), buttons[index]->get_pos_y(), buttons[index]->get_width(), buttons[index]->get_height() };
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game::get_instance()->m_renderer->renderer, 255, 255, 255, transparency_alpha - 20);
    SDL_RenderFillRect(Game::get_instance()->m_renderer->renderer, &highlight_box);
    SDL_SetRenderDrawBlendMode(Game::get_instance()->m_renderer->renderer, SDL_BLENDMODE_NONE);
    Game::get_instance()->m_renderer->update_screen();
}