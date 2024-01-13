#pragma once

#include <SDL2/SDL.h>

#ifndef SDL_PRACTICE_CAMERA_H
#define SDL_PRACTICE_CAMERA_H

class Camera {
public:
    SDL_Rect camera_rect;
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    int origin_x;
    int origin_y;

    Camera();
    float calculate_smoothing(float obj_distance, float min_smoothing, float max_smoothing, float range_limit, float dead_zone);
    void center_on_object(const SDL_Rect& object_rect, int Y_OFFSET, int X_OFFSET);
    void reset();

};


#endif //SDL_PRACTICE_CAMERA_H
