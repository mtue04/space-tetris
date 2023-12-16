#include "render.h"

void Renderer::init(SDL_Window* window) {
	small_font = TTF_OpenFont("assets/munro-small.ttf", 30);
	medium_font = TTF_OpenFont("assets/munro-small.ttf", 40);

	if (small_font == nullptr || medium_font == nullptr)
		cerr << "Tetris could not load fonts. SDL_ttf error: " << TTF_GetError() << endl;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	clear_screen();
}

Renderer::~Renderer() {
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
}

void Renderer::clear_screen() {
	SDL_SetRenderDrawColor(renderer, background_r_light, background_g_light, background_b_light, 0xFF);
	SDL_RenderClear(renderer);
}

void Renderer::render_texture(Texture* texture, int x, int y) {
	texture->render_center(x, y);
}

void Renderer::update_screen() {
	SDL_RenderPresent(renderer);
}

/////////////////////////////////////////////////////////////////////

Texture::Texture() : texture(nullptr) {}

Texture::~Texture() {
	if (texture != nullptr) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
		width = 0;
		height = 0;
	}
}

void Texture::free() {
	if (texture != nullptr) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
		width = 0;
		height = 0;
	}
}

int Texture::get_width() { return width; }

int Texture::get_height() { return height; }

bool Texture::load_image(const string& path) {
	free();
	SDL_Surface* temp_surf = IMG_Load(path.c_str());
	if (temp_surf == nullptr) {
		cerr << "Texture: Could not load image from path: " << path << endl;
		return false;
	} else {
		texture = SDL_CreateTextureFromSurface(Game::get_instance()->m_renderer->renderer, temp_surf);
		width = temp_surf->w;
		height = temp_surf->h;
		SDL_FreeSurface(temp_surf);
	}
	return true;
}

bool Texture::load_text(const string& text, TTF_Font* font, SDL_Color text_color) {
	free();
	SDL_Surface* text_surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), text_color, width_window);
	if (text_surface == nullptr) {
		cerr << "Could not create surface from text! SDL_ttf error: " << TTF_GetError() << endl;
		return false;
	} else {
		texture = SDL_CreateTextureFromSurface(Game::get_instance()->m_renderer->renderer, text_surface);
		if (texture == nullptr) {
			cerr << "Could not create texture from rendered text! SDL error: " << SDL_GetError() << endl;
			SDL_FreeSurface(text_surface);
			return false;
		} else {
			width = text_surface->w;
			height = text_surface->h;
		}
	}
	SDL_FreeSurface(text_surface);
	return true;
}

void Texture::render(int x, int y, SDL_Rect* clip) {
	SDL_Rect r = { x, y, width, height };
	if (clip != nullptr) {
		r.w = clip->w;
		r.h = clip->h;
	}
	SDL_RenderCopy(Game::get_instance()->m_renderer->renderer, texture, clip, &r);
}

void Texture::render_center(int x, int y) {
	SDL_Rect r = { x - (width / 2), y - (height / 2), width, height };
	SDL_RenderCopy(Game::get_instance()->m_renderer->renderer, texture, nullptr, &r);
}

void Texture::set_alpha_mode(Uint8 alpha) {
	SDL_SetTextureAlphaMod(texture, alpha);
}