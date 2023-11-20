#pragma once

#include <SDL2/SDL.h>

#ifndef SDL_PRACTICE_CAMERA_H
#define SDL_PRACTICE_CAMERA_H

class Camera {
public:
    SDL_Rect camera_rect;
    int screen_width;
    int screen_height;
    int origin_x;
    int origin_y;

    Camera();
    float calculate_smoothing(int obj_distance, float min_smoothing, float max_smoothing, float range_limit);
    void center_on_object(const SDL_Rect& object_rect);
    void reset();

};


#endif //SDL_PRACTICE_CAMERA_H
