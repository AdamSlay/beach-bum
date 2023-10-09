#include <fstream>
#include <string>

#include <SDL2/SDL_image.h>
#include "nlohmann/json.hpp"

#include "Level.h"

Level::Level(SDL_Renderer* _renderer, std::vector<SDL_Rect>& _colliders)
        : renderer(_renderer), colliders(_colliders), platforms() {

    std::ifstream config_file("../etc/level_config.json");
    nlohmann::json config;
    config_file >> config;

    TILE_WIDTH = config["TILE_WIDTH"];
    TILE_HEIGHT = config["TILE_HEIGHT"];
    SCREEN_WIDTH = config["SCREEN_WIDTH"];
    SCREEN_HEIGHT = config["SCREEN_HEIGHT"];
    PARALLAX_FACTOR = config["PARALLAX_FACTOR"];
    PLATFORM_WIDTH = config["PLATFORM_WIDTH"];
    PLATFORM_HEIGHT = config["PLATFORM_HEIGHT"];
    GROUND_HEIGHT = config["GROUND_HEIGHT"];
    GROUND_WIDTH_MIN = config["GROUND_WIDTH_MIN"];
    GROUND_WIDTH_MAX = config["GROUND_WIDTH_MAX"];
    GAP_WIDTH_MIN = config["GAP_WIDTH_MIN"];
    GAP_WIDTH_MAX = config["GAP_WIDTH_MAX"];
    X_MIN = config["X_MIN"];
    Y_MIN = config["Y_MIN"];
    Y_MAX = config["Y_MAX"];
    PLATFORM_COUNT = config["PLATFORM_COUNT"];
    PLATFORM_TYPES = config["PLATFORM_TYPES"];
    PLATFORM_SCALE_FACTOR = config["PLATFORM_SCALE_FACTOR"];

    // Initialize random number generators
    generator = std::default_random_engine(std::time(nullptr));
    plat_distributionX = std::uniform_int_distribution<int>(X_MIN, 100);
    plat_distributionY = std::uniform_int_distribution<int>(Y_MIN, Y_MAX);
    bg_distributionX = std::uniform_int_distribution(0, 3);
    bg_distributionY = std::uniform_int_distribution(0, 3);
    ground_distribution = std::uniform_int_distribution<int>(GROUND_WIDTH_MIN, GROUND_WIDTH_MAX);
    gap_distribution = std::uniform_int_distribution<int>(GAP_WIDTH_MIN, GAP_WIDTH_MAX);

    // TODO: remove this and pick random platform type for each platform in the generate_platform function
    std::uniform_int_distribution<int> distributionPlatform(0, PLATFORM_COUNT - 1);
    platform_type = distributionPlatform(generator) % 4;

    nextColumnX = 0 - SCREEN_WIDTH;
    bgSpriteSheet = IMG_LoadTexture(renderer, "../assets/bb_90s_pattern_dark.png");
    background = generateTileableBackground();

    // Initialize platforms
    std::string platform_path = "../assets/test_platforms.png";
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
    last_platform = {100, 400, 100, 100};  // the pre-first platform
    last_ground = {0, 350, 0, 500};  // the pre-first ground

    // Generate the first platform and ground
    generate_ground();
    generate_platform();
}

Level::~Level() {
    SDL_DestroyTexture(background);
    background = nullptr;
}

void Level::generate_ground() {
    SDL_Rect new_ground;
    int potential_new_x = last_ground.x + last_ground.w + gap_distribution(generator);
    new_ground.x = potential_new_x;
    new_ground.y = 350;
    new_ground.w = ground_distribution(generator);
    new_ground.h = GROUND_HEIGHT;

    // Add to grounds
    grounds.push_back(new_ground);

    // Add to colliders
    colliders.push_back(new_ground);

    // last_ground is now this current ground
    last_ground = new_ground;
}

void Level::generate_platform() {
    SDL_Rect new_platform;
    int potential_new_x = last_platform.x + last_platform.w + plat_distributionX(generator);
    new_platform.x = potential_new_x;
    new_platform.y = plat_distributionY(generator);
    new_platform.w = PLATFORM_WIDTH * PLATFORM_SCALE_FACTOR;
    new_platform.h = PLATFORM_HEIGHT * PLATFORM_SCALE_FACTOR;

    // Add to colliders
    colliders.push_back(new_platform);

    // Add to platforms
    platforms.push_back(new_platform);

    // last_platform is now this current platform
    last_platform = new_platform;
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
        SDL_Rect render_rect = platform;
        render_rect.x -= camera.camera_rect.x;
        render_rect.y -= camera.camera_rect.y;

        std::tuple<int, int> render_location = {render_rect.x, render_rect.y};
        platform_texture.render(render_location, renderer, &platform_sprite_clips[platform_type], 0, PLATFORM_SCALE_FACTOR);
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

int Level::get_next_column_x() {
    return nextColumnX;
}

void Level::increment_next_column_x() {
    nextColumnX += TILE_WIDTH;
}