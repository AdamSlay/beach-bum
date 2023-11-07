// utils.h
#ifndef UTILS_H
#define UTILS_H

#include "SDL2/SDL.h"
#include "Player.h"
#include "Camera.h"
#include "Level.h"

void run_game_loop(Player& player, Level& level, Camera& camera, SDL_Renderer* renderer, std::vector<SDL_Rect>& colliders);
void handle_keyboard_events(Player& player, SDL_Event& e, bool& quit);
void close(SDL_Renderer* renderer, SDL_Window* window);
void render_player_collider(Player& player, SDL_Renderer* renderer, Camera& camera);
bool initialize_resources(SDL_Renderer*& renderer, SDL_Window*& window);
bool check_collision(SDL_Rect a, SDL_Rect b);
bool check_collision_yax(SDL_Rect a, SDL_Rect b);


#endif // UTILS_H
