#include <ctime>
#include <iostream>
#include <random>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "Camera.h"
#include "Player.h"
#include "Texture.h"

// Constants
const int ORIGIN_X = 0;
const int ORIGIN_Y = 0;
const int SCREEN_WIDTH = 770;
const int SCREEN_HEIGHT = 420;
const int LEVEL_WIDTH = 1000;
const int LEVEL_HEIGHT = 800;
const int FPS = 60;
const int FRAME_DURATION = 1000 / FPS;
const int RIGHT = 0;
const int LEFT = 1;
const float PARALLAX_FACTOR = 0.9f;  // less than 1 to make the background move slower

// Define platform generation parameters
const int PLATFORM_WIDTH = 128;
const int PLATFORM_HEIGHT = 32;
const int X_MIN = 50;
const int X_MAX = LEVEL_WIDTH - PLATFORM_WIDTH; // ensure platform doesn't exceed level bounds
const int Y_MIN = 200;
const int Y_MAX = 280; // ensure platform doesn't exceed level bounds
const int PLATFORM_COUNT = 3; // number of platforms to generate
int platform_type;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Texture bg_texture;
Texture platform_texture;
SDL_Texture* background;
SDL_Rect bg_dest_rect;
SDL_Rect platform_sprite_clips[4];

bool init() {
    /**
     * Initialize SDL and create window
     */

    //Initialization flag
    bool success = true;

    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else {
        window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if(window == nullptr) {
            std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            success = false;
        }
        else {
            // initialize renderer
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (renderer == nullptr) {
                std::cout << "Renderer could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    std::cout << "SDL_image could not be initialized!" << std::endl;
                    std::cout << "SDL_Error: " << IMG_GetError() << std::endl;
                }
            }
        }
    }
    return success;
}

SDL_Texture* generateBackground(SDL_Renderer* bgrenderer) {
    // BG tile
    int dest_level_width = LEVEL_WIDTH;
    int dest_level_height = LEVEL_HEIGHT;
    bg_dest_rect.w = dest_level_width;
    bg_dest_rect.h = dest_level_height;
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
    SDL_Texture* levelTexture = SDL_CreateTexture(bgrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_WIDTH, LEVEL_HEIGHT);

    // Set the renderer target to the new texture
    SDL_SetRenderTarget(bgrenderer, levelTexture);

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
    for(int x = 0; x < LEVEL_WIDTH; x += tileWidth) {
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
    SDL_SetRenderTarget(renderer, NULL);

    return levelTexture;
}

bool loadMedia() {
    // Asset file paths
    std::string platform_path = "../assets/test_platforms.png";

    background = generateBackground(renderer);

    // load platform texture
    bool success = true;
    if (!platform_texture.loadFromFile(platform_path, renderer)) {
        std::cout << "Failed to load platform_texture!" << std::endl;
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

    return success;
}

void close() {
    // Free texture resources
    bg_texture.free();
    platform_texture.free();

    //Destroy window/renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;

    //Quit SDL/IMG subsystems
    IMG_Quit();
    SDL_Quit();
}

void render_background(SDL_Rect bg_dest_rect, Camera& camera, SDL_Texture* bg) {
    for(int x = 0; x < LEVEL_WIDTH; x += LEVEL_WIDTH) {
        // Loop to tile the bg image across the screen
        for(int y = 0; y < LEVEL_HEIGHT; y += LEVEL_HEIGHT) {
            bg_dest_rect.x = x - camera.camera_rect.x * PARALLAX_FACTOR;
            bg_dest_rect.y = y - camera.camera_rect.y * PARALLAX_FACTOR;
            SDL_RenderCopy(renderer, bg, nullptr, &bg_dest_rect);
        }
    }
}

void render_platforms(std::vector<SDL_Rect>& platforms, Camera& camera) {
    for (auto &platform : platforms) {
        // Loop to iterate through all platforms and render them
        SDL_Rect render_rect = platform;
        render_rect.x -= camera.camera_rect.x;
        render_rect.y -= camera.camera_rect.y;

        std::tuple<int, int> render_location = {render_rect.x, render_rect.y};
        platform_texture.render(render_location, renderer, &platform_sprite_clips[platform_type], 0, 0.55);
    }
}

void render_ground(Camera& camera, SDL_Rect ground) {
    SDL_Rect render_rect = ground;
    render_rect.x -= camera.camera_rect.x;
    render_rect.y -= camera.camera_rect.y;
    SDL_RenderFillRect(renderer, &render_rect);
}

int main( int argc, char* args[] ) {
    //Start up SDL and create window
    if(!init()) {
        std::cout << "Failed to initialize!" << std::endl;
        return 1;
    }

    //Load media
    if(!loadMedia()) {
        std::cout << "Failed to load media!" << std::endl;
        return 2;
    }

    // Camera
    SDL_Rect camera_rect = {ORIGIN_X, ORIGIN_Y, SCREEN_WIDTH, SCREEN_HEIGHT};
    Camera camera(camera_rect, LEVEL_WIDTH, LEVEL_HEIGHT);

    // Main loop vars
    Player player(renderer, "Player");
    SDL_Event e;
    Uint64 frame_start = SDL_GetTicks64();
    Uint64 frame_end{};

    /**
     * All of this block building and platform generation can be done in a Level class
     */
    SDL_Rect starting_block;
    starting_block.x = 100;
    starting_block.y = 400;
    starting_block.w = 100;
    starting_block.h = 100;
    SDL_Rect ground;
    ground.x = 100;
    ground.y = 350;
    ground.w = 800;
    ground.h = 500;
    std::vector<SDL_Rect> colliders {ground};
    std::vector<SDL_Rect> platforms {};

    // Use a random number generator
    std::default_random_engine generator(std::time(nullptr));
    std::uniform_int_distribution<int> distributionX(X_MIN, 100);
    std::uniform_int_distribution<int> distributionY(Y_MIN, Y_MAX);
    std::uniform_int_distribution<int> distributionPlatform(0, PLATFORM_COUNT - 1);
    platform_type = distributionPlatform(generator);

    auto& last_platform = starting_block;
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        SDL_Rect new_platform;
        int potential_new_x = last_platform.x + last_platform.w + distributionX(generator);
        new_platform.x = std::min(X_MAX, potential_new_x);
        new_platform.y = distributionY(generator);
        new_platform.w = PLATFORM_WIDTH;
        new_platform.h = PLATFORM_HEIGHT;
        std::cout << "New platform: " << new_platform.x << ", " << new_platform.y << std::endl;

        // Add to colliders
        colliders.push_back(new_platform);
        platforms.push_back(new_platform);
        last_platform = new_platform;
    }
    /**
     * The above block building and platform generation can be done in a Level class
     * You would basically take your level seed and build the level from that each time you load a level
     * the level seed could correspond with a config file that has the level structure data
     */


    // main loop
    bool quit = false;
    while(!quit) {
        // frame timing
        float delta_time = static_cast<float>(SDL_GetTicks64() - frame_start) / 1000.0f;
        frame_start = SDL_GetTicks64();

        while(SDL_PollEvent( &e) != 0) {
            if(e.type == SDL_QUIT) {
                // end runtime and close window if close button pressed
                quit = true;
            }
            else {
                // handle player input
                player.handle_event(e);
            }
        }

        // process player actions/movement
        player.move(delta_time, colliders);

        /**
         * The block below is all of the Level component rendering
         */
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        render_background(bg_dest_rect, camera, background);
        render_platforms(platforms, camera);
        render_ground(camera, ground);
        player.render(camera);
        SDL_RenderPresent(renderer);

        /**
         * Frame timing
         * Determine time it took to process this frame and delay if necessary to maintain constant frame rate
         */
        frame_end = SDL_GetTicks64();
        auto frame_time = frame_end - frame_start; // time it took to process this frame
        if (frame_time < FRAME_DURATION) {
            SDL_Delay(FRAME_DURATION - frame_time); // wait the remaining frame time
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}