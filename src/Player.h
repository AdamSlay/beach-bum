#pragma once

#include <vector>

#include <SDL2/SDL.h>

#include "Texture.h"
#include "Animator.h"

class Player {
public:
    std::string state;
    Player();
    void handle_event(SDL_Event& e);
    void move(float delta_time, std::vector<SDL_Rect>& objects);
    void render(int camera_x, int camera_y, Texture& texture, SDL_Renderer* renderer, SDL_Rect* clip) const;
    void jump();
    int get_x() const;
    int get_y() const;
    int get_width() const;
    int get_height() const;

private:
    const int width, height;
    const float velocity, initial_jump_velocity;
    bool jumping, grounded;
    int pos_x, pos_y, direction;
    float vel_x, vel_y;
    SDL_Rect collider;
    Animator animator;
};
