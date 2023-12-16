#include "definition.h"

// CONSTANTS TETRIS
int wait_time = 1000;
int wait_time_list[] = { 1500, 1000, 200, 50 };
int difficulty_index = 1;
bool ghost_block_enabled = true;

// SCORE
int your_score = 0;
Score::Score() {
    score = 0;
    lines = 0;
    high_score = load_high_score();
}

Score::~Score() {}

int Score::load_high_score() {
    ifstream file("assets/score.dat", ios::binary);
    if (file.is_open()) {
        file >> high_score;
        file.close();
    } else {
        cerr << "Score: Could not load file 'assets/score.dat'" << endl;
        return 0;
    }
    return high_score;
}

void Score::save_score() {
    ofstream file("assets/score.dat", ios::binary | ios::app);
    if (file.is_open()) {
        file << score << endl;
        file.close();
        
        ifstream read_file("assets/score.dat", ios::binary);
        if (read_file.is_open()) {
            vector<int> score_list;
            int temp;
            while (read_file >> temp) {
                score_list.push_back(temp);
            }
            read_file.close();
            sort(score_list.begin(), score_list.end(), greater<int>());
            
            ofstream write_file("assets/score.dat", ios::binary);
            if (write_file.is_open()) {
                for (int i = 0; i < score_list.size(); i++) {
                    write_file << score_list[i] << endl;
                }
                write_file.close();
            } else {
                cerr << "Score: Could not save file 'assets/score.dat'" << endl;
            }
        } else {
            cerr << "Score: Could not load file 'assets/score.dat'" << endl;
        }
    } else {
        cerr << "Score: Could not save file 'assets/score.dat'" << endl;
    }
}

int Score::get_high_score() { return high_score; }

int Score::get_score() { return score; }

int Score::get_lines() { return lines; }

void Score::update_score(int new_score) {
    score = new_score;
    if (score > high_score) {
        high_score = score;
    }
}

void Score::update_lines(int new_line) { lines = new_line; }

// MUSIC AND SOUNDS
bool is_music_sounds_on = true;

Sounds::Sounds() : music(nullptr), sound(nullptr) {}

void Sounds::load_music(const char* filename) {
    music = Mix_LoadMUS(filename);
    if (music == NULL) {
        cerr << "Music: Failed to load music. Error: " << Mix_GetError() << endl;
    }
}

void Sounds::load_sound(const char* filename) {
    sound = Mix_LoadWAV(filename);
    if (sound == NULL) {
        cerr << "Music: Failed to load sound. Error: " << Mix_GetError() << endl;
    }
}

void Sounds::play_music() {
    if (is_music_sounds_on)
        Mix_PlayMusic(music, -1);
    else
        Mix_HaltMusic();
}

void Sounds::play_sound() {
    if (is_music_sounds_on)
        Mix_PlayChannel(-1, sound, 0);
}

void Sounds::pause_music(bool pause) {
    if (pause)
        Mix_PauseMusic();
    else
        Mix_ResumeMusic();
}