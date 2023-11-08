// utils.h
#ifndef UTILS_H
#define UTILS_H

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "Player.h"
#include "Camera.h"
#include "Level.h"

void run_game_loop(SDL_Renderer* renderer,Player& player, Level& level, Camera& camera, TTF_Font* font);
void handle_keyboard_events(Player& player, SDL_Event& e, bool& quit);
void close(SDL_Renderer* renderer, SDL_Window* window);
void render_score(SDL_Renderer* renderer, const std::string& text, TTF_Font* font, SDL_Color color);
void render_player_collider(Player& player, SDL_Renderer* renderer, Camera& camera);
bool initialize_resources(SDL_Renderer*& renderer, SDL_Window*& window, TTF_Font*& font);
bool check_collision(SDL_Rect a, SDL_Rect b);
bool check_collision_yax(SDL_Rect a, SDL_Rect b);


#endif // UTILS_H
