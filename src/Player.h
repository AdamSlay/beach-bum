#pragma once

#include <vector>

#include <SDL2/SDL.h>

#include "Texture.h"

class Player {
public:
    Player();
    void handle_event(SDL_Event& e);
    void move(float delta_time, std::vector<SDL_Rect>& objects);
    void render(int camera_x, int camera_y, Texture& texture, SDL_Renderer* renderer, SDL_Rect viewport, SDL_Rect* clip, int direction) const;
    void jump();
    int get_x() const;
    int get_y() const;
    int get_width() const;
    int get_height() const;

private:
    const int width;
    const int height;
    const float velocity;
    const float initial_jump_velocity;
    int pos_x, pos_y;
    float vel_x, vel_y;
    SDL_Rect collider;
    bool jumping, grounded;
};
