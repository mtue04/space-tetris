#ifndef RENDER_H
#define RENDER_H

#include "header.h"

class Texture;
class Renderer;

class Renderer {
public:
    TTF_Font* small_font;
    TTF_Font* medium_font;
    TTF_Font* cd_font;

    SDL_Renderer* renderer = nullptr;
    void init(SDL_Window* window);
    ~Renderer();

    void clear_screen();
    void render_texture(Texture* texture, int x, int y);
    void update_screen();
};

class Texture {
private:
    SDL_Texture* texture;
public:
    Texture();
    ~Texture();
    void free();

    int width;
    int height;
    int get_width();
    int get_height();

    bool load_image(const string& path);
    bool load_text(const string& text, TTF_Font* font, SDL_Color text_color);
    void render(int x, int y, SDL_Rect* clip = nullptr);
    void render_center(int x, int y);
    void set_alpha_mode(Uint8 alpha);       // Transparency
};

#endif // !RENDER_H