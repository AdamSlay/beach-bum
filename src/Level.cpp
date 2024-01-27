#include <fstream>
#include <string>

#include <SDL2/SDL_image.h>
#include "nlohmann/json.hpp"

#include "Level.h"

Level::Level(SDL_Renderer* _renderer)
        : renderer(_renderer), platforms() {

    std::ifstream run_config_file("../etc/run_config.json");
    nlohmann::json run_config;
    run_config_file >> run_config;
    SCREEN_WIDTH = run_config["SCREEN_WIDTH"];
    SCREEN_HEIGHT = run_config["SCREEN_HEIGHT"];
    GROUND_LEVEL_Y = run_config["GROUND_LEVEL_Y"];

    std::ifstream config_file("../etc/level_config.json");
    nlohmann::json config;
    config_file >> config;
    TILE_WIDTH = config["TILE_WIDTH"];
    TILE_HEIGHT = config["TILE_HEIGHT"];
    PARALLAX_FACTOR = config["PARALLAX_FACTOR"];
    PLATFORM_WIDTH = config["PLATFORM_WIDTH"];
    PLATFORM_HEIGHT = config["PLATFORM_HEIGHT"];
    GROUND_HEIGHT = config["GROUND_HEIGHT"];
    GROUND_WIDTH_MIN = config["GROUND_WIDTH_MIN"];
    GROUND_WIDTH_MAX = config["GROUND_WIDTH_MAX"];
    GAP_WIDTH_MIN = config["GAP_WIDTH_MIN"];
    GAP_WIDTH_MAX = config["GAP_WIDTH_MAX"];
    PLAT_DIST_X_MIN = config["PLAT_DIST_X_MIN"];
    PLAT_DIST_X_MAX = config["PLAT_DIST_X_MAX"];
    PLAT_DIST_Y_MIN = config["PLAT_DIST_Y_MIN"];
    PLAT_DIST_Y_MAX = config["PLAT_DIST_Y_MAX"];
    PLATFORM_TYPES = config["PLATFORM_TYPES"];
    PLATFORM_SCALE_FACTOR = config["PLATFORM_SCALE_FACTOR"];
    BACKGROUND_SPRITE_SHEET_PATH = config["BACKGROUND_SPRITE_PATH"];
    PLATFORM_SPRITE_SHEET_PATH = config["PLATFORM_SPRITE_PATH"];
    colliders = std::vector<SDL_Rect>();

    // Initialize random number generators
    std::random_device rd;
    generator = std::default_random_engine(rd());
    plat_distributionX = std::uniform_int_distribution<int>(PLAT_DIST_X_MIN, PLAT_DIST_X_MAX);
    plat_distributionY = std::uniform_int_distribution<int>(PLAT_DIST_Y_MIN, PLAT_DIST_Y_MAX);
    plat_type_distribution = std::uniform_int_distribution<int>(0, PLATFORM_TYPES-1);
    bg_distributionX = std::uniform_int_distribution(0, 3);
    bg_distributionY = std::uniform_int_distribution(0, 3);
    ground_distributionX = std::uniform_int_distribution<int>(GROUND_WIDTH_MIN, GROUND_WIDTH_MAX);
    gap_distribution = std::uniform_int_distribution<int>(GAP_WIDTH_MIN, GAP_WIDTH_MAX);

    nextColumnX = 0 - SCREEN_WIDTH;
    bgSpriteSheet = IMG_LoadTexture(renderer, BACKGROUND_SPRITE_SHEET_PATH.c_str());
    background = generateTileableBackground();

    // Initialize platforms
    std::string platform_path = PLATFORM_SPRITE_SHEET_PATH;
    if (!platform_texture.loadFromFile(platform_path, renderer)) {
        throw std::runtime_error("Failed to load platform texture from " + platform_path);
    }
    else {
        for (int i = 0; i < PLATFORM_TYPES; i++) {
            platform_sprite_clips[i].x = 0;
            platform_sprite_clips[i].y = i * PLATFORM_HEIGHT;
            platform_sprite_clips[i].w = PLATFORM_WIDTH;
            platform_sprite_clips[i].h = PLATFORM_HEIGHT;
        }
    }
    last_platform = {100, 400, 0, 100};  // the pre-first platform
    last_terrain = {0, GROUND_LEVEL_Y, 700, 500};  // the pre-first ground
    last_ground = last_terrain;  // the pre-first terrain
    grounds.push_back(last_terrain);
    colliders.push_back(last_terrain);

}

Level::~Level() {
    SDL_DestroyTexture(background);
    background = nullptr;
}

void Level::update(int player_x, int camera_x) {
    // check if it's time to generate a new terrain object
    generate_terrain(player_x);

    // Check if it's time to generate a new background column
    if (camera_x >= get_next_column_x()) {
        generateBackgroundColumn();
        increment_next_column_x();
    }
}

void Level::generate_terrain(int player_x) {
    // Decide whether to generate a new ground or a new platform
    if (player_x > get_last_terrain().x - SCREEN_WIDTH) {
        int terrain_type = std::rand() % 2;
        if (terrain_type == 0) {
            generate_ground();
        } else {
            generate_platform();
        }
    }
}

void Level::generate_ground() {
    SDL_Rect new_ground;
    int potential_new_x = last_terrain.x + last_terrain.w + gap_distribution(generator);
    new_ground.x = potential_new_x;
    new_ground.y = plat_distributionY(generator);
    new_ground.w = ground_distributionX(generator);
    new_ground.h = GROUND_HEIGHT;

    grounds.push_back(new_ground);
    colliders.push_back(new_ground);
    last_terrain = new_ground;
    last_ground = new_ground;
}

void Level::generate_platform() {
    Platform new_platform;
    int new_plat_x = last_terrain.x + last_terrain.w + plat_distributionX(generator);
    new_platform.rect.x = new_plat_x;
    new_platform.rect.y = plat_distributionY(generator);
    new_platform.rect.w = PLATFORM_WIDTH * PLATFORM_SCALE_FACTOR;  // scale the platform's collider to match rendered scale
    new_platform.rect.h = PLATFORM_HEIGHT * PLATFORM_SCALE_FACTOR;  // scale the platform's collider to match rendered scale
    new_platform.type = plat_type_distribution(generator);

    colliders.push_back(new_platform.rect);
    platforms.push_back(new_platform);
    last_terrain = new_platform.rect;
    last_platform = new_platform.rect;
}

SDL_Texture* Level::generateTileableBackground() {
    // Create a new texture to hold the level background
    SDL_Texture* levelTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    return levelTexture;
}

void Level::generateBackgroundColumn() {
    // Create a new column texture
    SDL_Texture* columnTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TILE_WIDTH, SCREEN_HEIGHT);

    // Set the renderer target to the new texture
    SDL_SetRenderTarget(renderer, columnTexture);

    SDL_Rect srcRect = {0, 0, TILE_WIDTH, TILE_HEIGHT};
    SDL_Rect destRect = {0, 0, TILE_WIDTH, TILE_HEIGHT};

    // For each tile space in the column
    for(int y = 0; y < SCREEN_HEIGHT; y += TILE_HEIGHT) {
        // Randomly select a tile from the sprite sheet
        srcRect.y = bg_distributionY(generator) * TILE_HEIGHT;
        srcRect.x = bg_distributionX(generator) * TILE_WIDTH;

        // Copy the tile to the column texture
        destRect.y = y;
        SDL_RenderCopy(renderer, bgSpriteSheet, &srcRect, &destRect);
    }

    // Reset the renderer target
    SDL_SetRenderTarget(renderer, nullptr);

    // Append the column texture to the right side of the background texture
    backgroundColumns.push_back(columnTexture);
}

void Level::render(Camera camera) {
    render_background(camera);
    render_ground(camera);
    render_platforms(camera);
}

void Level::render_ground(Camera &camera) {
    for (auto &ground : grounds) {
        SDL_Rect render_rect = ground;
        render_rect.x -= camera.camera_rect.x;
        render_rect.y -= camera.camera_rect.y;
        SDL_RenderFillRect(renderer, &render_rect);
    }
}


void Level::render_platforms(Camera &camera) {
    for (auto &platform : platforms) {
        // Loop to iterate through all platforms and render them
        SDL_Rect render_rect = platform.rect;
        render_rect.x -= camera.camera_rect.x;
        render_rect.y -= camera.camera_rect.y;

        std::tuple<int, int> render_location = {render_rect.x, render_rect.y};
        platform_texture.render(render_location, renderer, &platform_sprite_clips[platform.type], 0, PLATFORM_SCALE_FACTOR);
    }
}

void Level::render_background(Camera &camera) {
    int x = 0;
    for(auto& columnTexture : backgroundColumns) {
        SDL_Rect destRect = {static_cast<int>(x - camera.camera_rect.x * PARALLAX_FACTOR), 0, TILE_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, columnTexture, nullptr, &destRect);
        x += TILE_WIDTH;
    }
}

std::vector<SDL_Rect> Level::get_colliders() {
    return colliders;
}

SDL_Rect Level::get_last_platform() {
    return last_platform;
}

SDL_Rect Level::get_last_ground() {
    return last_ground;
}

SDL_Rect Level::get_last_terrain() {
    return last_terrain;
}

int Level::get_next_column_x() {
    return nextColumnX;
}

void Level::increment_next_column_x() {
    nextColumnX += TILE_WIDTH;
}
