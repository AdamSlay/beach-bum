#include <string>
#include <iostream>

#include <SDL2/SDL_image.h>

#include "Level.h"
#include "Texture.h"

const float PARALLAX_FACTOR = 0.9f;
const int LEVEL_HEIGHT = 800;
const int PLATFORM_WIDTH = 128;
const int PLATFORM_HEIGHT = 32;
const int X_MIN = 50;
const int Y_MIN = 200;
const int Y_MAX = 250; // ensure platform doesn't exceed level bounds
const int PLATFORM_COUNT = 24; // number of platforms to generate
int platform_type;

SDL_Rect bg_dest_rect;
Texture platform_texture;
SDL_Rect platform_sprite_clips[4];
SDL_Texture *background;

Level::Level(SDL_Renderer* _renderer, std::vector<SDL_Rect>& _colliders)
        : renderer(_renderer), colliders(_colliders), platforms() {
    // Initialize background
    bg_dest_rect.w = 5000;
    bg_dest_rect.h = LEVEL_HEIGHT;
    bg_dest_rect.x = 0;
    bg_dest_rect.y = 0;
    background = generateBackground();

    // Initialize platforms
    std::string platform_path = "../assets/test_platforms.png";
    bool success = true;
    if (!platform_texture.loadFromFile(platform_path, renderer)) {
        success = false;
    }
    else {
        for (int i = 0; i < 4; i++) {
            platform_sprite_clips[i].x = 0;
            platform_sprite_clips[i].y = i * 64;
            platform_sprite_clips[i].w = 256;
            platform_sprite_clips[i].h = 64;
        }
    }

    last_platform.x = 100;
    last_platform.y = 400;
    last_platform.w = 100;
    last_platform.h = 100;
    SDL_Rect ground;
    ground.x = 100;
    ground.y = 350;
    ground.w = 8000;
    ground.h = 500;
    colliders.push_back(ground);

    // Initialize random number generator
    generator = std::default_random_engine(std::time(nullptr));
    distributionX = std::uniform_int_distribution<int>(X_MIN, 100);
    distributionY = std::uniform_int_distribution<int>(Y_MIN, Y_MAX);
    std::uniform_int_distribution<int> distributionPlatform(0, PLATFORM_COUNT - 1);
    platform_type = distributionPlatform(generator) % 4;

    // Generate the first platform
    generate_platform();
}

Level::~Level() {
    SDL_DestroyTexture(background);
    background = nullptr;
}

void Level::generate_platform() {
    SDL_Rect new_platform;
    int potential_new_x = last_platform.x + last_platform.w + distributionX(generator);
    new_platform.x = potential_new_x;
    new_platform.y = distributionY(generator);
    new_platform.w = PLATFORM_WIDTH;
    new_platform.h = PLATFORM_HEIGHT;

    // Add to colliders
    colliders.push_back(new_platform);

    // Add to platforms
    platforms.push_back(new_platform);

    // last_platform is now this current platform
    last_platform = new_platform;
}

void Level::render(Camera camera) {
    render_background(camera);
    render_ground(camera, colliders[0]);
    render_platforms(camera, platforms);
}

void Level::render_background(Camera &camera) {
    for(int x = 0; x < 5000; x += 5000) {
        // Loop to tile the bg image across the screen
        for(int y = 0; y < LEVEL_HEIGHT; y += LEVEL_HEIGHT) {
            bg_dest_rect.x = x - camera.camera_rect.x * PARALLAX_FACTOR;
            bg_dest_rect.y = y - camera.camera_rect.y * PARALLAX_FACTOR;
            SDL_RenderCopy(renderer, background, nullptr, &bg_dest_rect);
        }
    }
}

void Level::render_ground(Camera &camera, SDL_Rect& ground) {
    SDL_Rect render_rect = ground;
    render_rect.x -= camera.camera_rect.x;
    render_rect.y -= camera.camera_rect.y;
    SDL_RenderFillRect(renderer, &render_rect);
}

void Level::render_platforms(Camera &camera, std::vector<SDL_Rect> &platforms) {
    for (auto &platform : platforms) {
        // Loop to iterate through all platforms and render them
        SDL_Rect render_rect = platform;
        render_rect.x -= camera.camera_rect.x;
        render_rect.y -= camera.camera_rect.y;

        std::tuple<int, int> render_location = {render_rect.x, render_rect.y};
        platform_texture.render(render_location, renderer, &platform_sprite_clips[platform_type], 0, 0.55);
    }
}

SDL_Texture* Level::generateBackground() {

    // BG tile
    bg_dest_rect.w = 5000;
    bg_dest_rect.h = LEVEL_HEIGHT;
    bg_dest_rect.x = 0;
    bg_dest_rect.y = 0;

    SDL_Texture* bgSpriteSheet = nullptr;
    std::string spriteSheetPath = "../assets/bb_90s_pattern_dark.png"; // Path to your sprite sheet
    bgSpriteSheet = IMG_LoadTexture(renderer, spriteSheetPath.c_str());

    if (bgSpriteSheet == nullptr) {
        std::cout << "Failed to load sprite sheet texture image!" << std::endl;
        std::cout << "SDL_image Error: " << IMG_GetError() << std::endl;
    }

    const int tileWidth = 64;
    const int tileHeight = 64;

    // Create a new texture to hold the level background
    SDL_Texture* levelTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 5000, LEVEL_HEIGHT);

    // Set the renderer target to the new texture
    SDL_SetRenderTarget(renderer, levelTexture);

    // Get the number of tiles in the sprite sheet
    int tilesX = 256 / tileWidth;
    int tilesY = 256 / tileHeight;

    // Use a random number generator
    std::default_random_engine generator(std::time(nullptr));
    std::uniform_int_distribution<int> distributionX(0, tilesX - 1);
    std::uniform_int_distribution<int> distributionY(0, tilesY - 1);

    SDL_Rect srcRect = {0, 0, tileWidth, tileHeight};
    SDL_Rect destRect = {0, 0, tileWidth, tileHeight};

    // For each tile space in the level
    for(int x = 0; x < 5000; x += tileWidth) {
        for(int y = 0; y < LEVEL_HEIGHT; y += tileHeight) {
            // Randomly select a tile from the sprite sheet
            srcRect.x = distributionX(generator) * tileWidth;
            srcRect.y = distributionY(generator) * tileHeight;

            // Copy the tile to the level texture
            destRect.x = x;
            destRect.y = y;
            destRect.w = tileWidth;
            destRect.h = tileHeight;
            SDL_RenderCopy(renderer, bgSpriteSheet, &srcRect, &destRect);
        }
    }

    // Reset the renderer target
    SDL_SetRenderTarget(renderer, nullptr);

    return levelTexture;
}

std::vector<SDL_Rect> Level::get_colliders() {
    return colliders;
}

SDL_Rect Level::get_last_platform() {
    return last_platform;
}