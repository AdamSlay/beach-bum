#include <SDL2/SDL.h>
#include <random>
#include <vector>

#include "Camera.h"

#ifndef SDL_PRACTICE_LEVEL_H
#define SDL_PRACTICE_LEVEL_H


class Level {
    public:
    int TILE_WIDTH{64};
    int TILE_HEIGHT{64};
    int SCREEN_WIDTH{770};
    int SCREEN_HEIGHT{420};
    int nextColumnX;
    Level(SDL_Renderer* _renderer, std::vector<SDL_Rect>& _colliders);
    ~Level();
    void generate_platform();
    void generateBackgroundColumn();
    void render(Camera camera);
    std::vector<SDL_Rect> get_colliders();
    SDL_Rect get_last_platform();

    private:
    SDL_Renderer* renderer;
    std::vector<SDL_Rect> colliders;
    std::vector<SDL_Rect> platforms;
    SDL_Rect last_platform;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> plat_distributionX;
    std::uniform_int_distribution<int> plat_distributionY;
    std::uniform_int_distribution<int> bg_distributionX;
    std::uniform_int_distribution<int> bg_distributionY;
    SDL_Texture* bgSpriteSheet;
    std::vector<SDL_Texture*> backgroundColumns;

    void render_background(Camera& camera);
    void render_ground(Camera& camera, SDL_Rect& ground);
    void render_platforms(Camera& camera, std::vector<SDL_Rect>& platforms);
    SDL_Texture* generateTileableBackground();
};


#endif //SDL_PRACTICE_LEVEL_H
