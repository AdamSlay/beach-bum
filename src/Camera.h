#pragma once

#include <SDL2/SDL.h>

#include "Player.h"

#ifndef SDL_PRACTICE_CAMERA_H
#define SDL_PRACTICE_CAMERA_H

class Camera {
public:
    SDL_Rect rect;
    int level_width;
    int level_height;

    Camera(SDL_Rect rect, int level_width, int level_height);
    float calculate_smoothing(int edge_distance, float min_smoothing, float max_smoothing);
    void center_on_player(const Player& player);

};


#endif //SDL_PRACTICE_CAMERA_H
