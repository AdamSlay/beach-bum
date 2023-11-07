// utils.h
#ifndef UTILS_H
#define UTILS_H

#include "SDL2/SDL.h"
#include "Player.h"
#include "Camera.h"

void close(SDL_Renderer* renderer, SDL_Window* window);
void render_player_collider(Player& player, SDL_Renderer* renderer, Camera& camera);
bool initialize_resources(SDL_Renderer*& renderer, SDL_Window*& window);
bool check_collision(SDL_Rect a, SDL_Rect b);
bool check_collision_yax(SDL_Rect a, SDL_Rect b);


#endif // UTILS_H
