#include <SDL2/SDL.h>
#include <vector>

#include "Camera.h"

#ifndef SDL_PRACTICE_LEVEL_H
#define SDL_PRACTICE_LEVEL_H


class Level {
    public:
    Level(SDL_Renderer* _renderer);
    void render(Camera* camera);

    private:
    SDL_Renderer* renderer;
    std::vector<SDL_Rect> collision_objects;
    void render_background(Camera& camera);
    void render_ground(Camera& camera, SDL_Rect& ground);
    void render_platforms(Camera& camera, std::vector<SDL_Rect>& platforms);
    SDL_Texture* generateBackground();





};


#endif //SDL_PRACTICE_LEVEL_H