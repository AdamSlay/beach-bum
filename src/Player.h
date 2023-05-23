#include <vector>

#include <SDL2/SDL.h>

#include "Texture.h"

#ifndef SDL_PRACTICE_PLAYER_H
#define SDL_PRACTICE_PLAYER_H


class Player {
public:
    Player();
    void handle_event(SDL_Event& e);
    void move(float delta_time, std::vector<SDL_Rect>& objects);
    void render(Texture& texture, SDL_Renderer* renderer, SDL_Rect viewport, SDL_Rect* clip, int direction) const;
    void jump();

private:
    const int width;
    const int height;
    const float velocity;
    const float initial_jump_velocity;
    float sustained_jump_velocity;
    int pos_x, pos_y;
    float vel_x, vel_y;
    SDL_Rect collider;
    bool jumping, grounded;
};


#endif //SDL_PRACTICE_PLAYER_H
