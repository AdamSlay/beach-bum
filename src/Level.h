#include <SDL2/SDL.h>
#include <random>
#include <vector>

#include "Camera.h"
#include "Texture.h"

#ifndef SDL_PRACTICE_LEVEL_H
#define SDL_PRACTICE_LEVEL_H

struct Platform {
    SDL_Rect rect;
    int type;
};

class Level {
public:
    Level(SDL_Renderer* _renderer);
    ~Level();
    void update(int player_x, int camera_x);
    void generate_ground();
    void generate_platform();
    void generate_terrain(int player_x);
    void generateBackgroundColumn();
    void render(Camera camera);
    std::vector<SDL_Rect> get_colliders();
    SDL_Rect get_last_platform();
    SDL_Rect get_last_ground();
    SDL_Rect get_last_terrain();
    int get_next_column_x();
    void increment_next_column_x();

private:
    int TILE_WIDTH;
    int TILE_HEIGHT;
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    float PARALLAX_FACTOR;
    int PLATFORM_WIDTH;
    int PLATFORM_HEIGHT;
    float PLATFORM_SCALE_FACTOR;
    int GROUND_HEIGHT;
    int GROUND_WIDTH_MIN;
    int GROUND_WIDTH_MAX;
    int GAP_WIDTH_MIN;
    int GAP_WIDTH_MAX;
    int PLAT_DIST_X_MIN;
    int PLAT_DIST_X_MAX;
    int PLAT_DIST_Y_MIN;
    int PLAT_DIST_Y_MAX;
    int PLATFORM_TYPES;
    int GROUND_LEVEL_Y;
    std::string BACKGROUND_SPRITE_SHEET_PATH;
    std::string PLATFORM_SPRITE_SHEET_PATH;
    int nextColumnX;
    Texture platform_texture;
    SDL_Rect platform_sprite_clips[4];
    SDL_Texture *background;
    SDL_Renderer* renderer;
    std::vector<SDL_Rect> colliders;
    std::vector<Platform> platforms;
    std::vector<SDL_Rect> grounds;
    SDL_Rect last_ground;
    SDL_Rect last_platform;
    SDL_Rect last_terrain;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> plat_distributionX;
    std::uniform_int_distribution<int> plat_distributionY;
    std::uniform_int_distribution<int> plat_type_distribution;
    std::uniform_int_distribution<int> bg_distributionX;
    std::uniform_int_distribution<int> bg_distributionY;
    std::uniform_int_distribution<int> ground_distributionX;
    std::uniform_int_distribution<int> gap_distribution;
    SDL_Texture* bgSpriteSheet;
    std::vector<SDL_Texture*> backgroundColumns;

    void render_background(Camera& camera);
    void render_ground(Camera &camera);
    void render_platforms(Camera& camera);
    SDL_Texture* generateTileableBackground();
};

#endif //SDL_PRACTICE_LEVEL_H