#ifndef DEFINITION_H
#define DEFINITION_H

// LIBRARY USED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <Windows.h>

// NAMESPACE DECLARATION
using namespace std;

// WINDOW DECLARATION
const char window_title[] = "TETRIS";

// LOGICAL RESOLUTION OF GAME
const int width_window = 960;
const int height_window = 540;

// BOARD CLASS
const int width_to_playfield = 363;                                             // in pixels
const int height_to_playfield = 51;                                             // in pixels
const int block_size = 24;                                                      // in pixels
const int playfield_width = 10;                                                 // in blocks
const int true_playfield_height = 20;                                           // in blocks
const int playfield_height = 22;                                                // The playfield+2 rows directly above it for spawning the Tetrominos
const int frame_width = 9;                                                      // Frame that surrounds the playfield; in pixels
const int frame_sprite_size = 12;                                               // Size of each sprite clip for the playfield frame
const int board_height = 3;                                                     // distance from bottom where the playfield begins; in pixels
const int matrix_blocks = 5;                                                    // matrix that holds tetromino info; in blocks

// GAMESTATE CLASS
const int next_box_x = 750;                                                     // Horizontal distance from top left corner; in pixels
const int next_box_y = 25;                                                      // Vertical distance from top left corner; in pixels
const int hold_box_x = 750;                                                     // Horizontal distance from top left corner; in pixels
const int hold_box_y = 50;                                                      // Vertical distance from top left corner; in pixels

// CONSTANTS TETRIS
extern int wait_time;                                                           // Time in milliseconds
extern int wait_time_list[];
extern int difficulty_index;
extern bool ghost_block_enabled;                                                // Ghost block enabled or not

// VISUALS
const SDL_Color default_text_color = { 0xFF, 0xDE, 0x59, 0xFF };                // Default text color (yellow)
const SDL_Color blue_text_color = { 50, 169, 247, 255 };
const SDL_Color red_text_color = { 0xFF, 0x00, 0x00, 0xFF };
const Uint8 background_r_light = 0xF9;                                          // Light background color
const Uint8 background_g_light = 0xE6;
const Uint8 background_b_light = 0xE6;
const Uint8 transparency_alpha = 100;                                           // Transparency alpha (with 255 being no transparency) 

// SCORE
const int score_per_line = 10;                                                  // Score per line cleared (10 for Easy, 20 for Medium, 30 for Hard)
extern int your_score;
class Score {                                                                   // Score class (Handle everything related to score)
private:
    int score;
    int lines;
    int high_score;
public:
    Score();
    ~Score();

    // Handle file
    int load_high_score();
    void save_score();

    int get_high_score();
    int get_score();
    int get_lines();
    void update_score(int new_score);
    void update_lines(int new_line);
};

// MUSIC AND SOUNDS
extern bool is_music_sounds_on;
class Sounds {                                                                 // Sounds class (Handle everything related to sounds)
    Mix_Music* music;
    Mix_Chunk* sound;
public:
    Sounds();
    void load_music(const char* filename);
    void load_sound(const char* filename);
    void play_music();
    void play_sound();
    void pause_music(bool pause);
};

#endif // !_DEFINITION_H