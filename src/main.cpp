#include <ctime>
#include <iostream>
#include <random>
#include <string>
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
const int RUNNING_ANIMATION_FRAMES = 8;
const int RIGHT = 0;
const int LEFT = 1;
const float PARALLAX_FACTOR = 0.9f;  // less than 1 to make the background move slower

// Define platform generation parameters
const int PLATFORM_WIDTH = 100;
const int PLATFORM_HEIGHT = 12;
const int X_MIN = 100;
const int X_MAX = LEVEL_WIDTH - PLATFORM_WIDTH; // ensure platform doesn't exceed level bounds
const int Y_MIN = 200;
const int Y_MAX = 300; // ensure platform doesn't exceed level bounds
const int PLATFORM_COUNT = 3; // number of platforms to generate

int player_direction{};  // 0 = right, 1 = left
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Texture bg_texture;
Texture minimap_texture;
Texture char_sprite_sheet;
SDL_Rect char_sprite_clips[RUNNING_ANIMATION_FRAMES];

bool init() {
    /*
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

bool loadMedia() {
    // Asset file paths
    std::string bg_path = "../assets/bb_90s_pattern_dark.png";
    std::string minimap_path = "../assets/up.png";
    std::string char_path = "../assets/bb_run_sheet.png";

    bool success = true;
    // load background texture
    if (!bg_texture.loadFromFile(bg_path, renderer)) {
        std::cout << "Failed to load bg_texture!" << std::endl;
        success = false;
    }
    // load character texture
    if (!char_sprite_sheet.loadFromFile(char_path, renderer)) {
        std::cout << "Failed to load char_sprite_sheet!" << std::endl;
        success = false;
    }
    else {
        // create char sprite clips
        for (int i = 0; i < 8; i++) {
            char_sprite_clips[i].x = (i * 64) + 12;
            char_sprite_clips[i].y = 52;
            char_sprite_clips[i].w = 32;
            char_sprite_clips[i].h = 48;
        }
    }

    return success;
}

void close() {
    // Free texture resources
    bg_texture.free();
    minimap_texture.free();
    char_sprite_sheet.free();

    //Destroy window/renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;

    //Quit SDL/IMG subsystems
    IMG_Quit();
    SDL_Quit();
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

    // Viewport & Camera
    SDL_Rect full_viewport = {ORIGIN_X, ORIGIN_Y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect camera_rect = {ORIGIN_X, ORIGIN_Y, SCREEN_WIDTH, SCREEN_HEIGHT};
    Camera camera(camera_rect, LEVEL_WIDTH, LEVEL_HEIGHT);

    // Main loop vars
    SDL_Event e;
    Player player;
    int frame = 0;
    Uint64 frame_start = SDL_GetTicks64();
    Uint64 frame_end{};

    // starting_block and ground rects for testing collision
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
    std::vector<SDL_Rect> colliders = {ground};

    // Use a random number generator
    std::default_random_engine generator(std::time(nullptr));
    std::uniform_int_distribution<int> distributionX(50, 100);
    std::uniform_int_distribution<int> distributionY(Y_MIN, Y_MAX);

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
        last_platform = new_platform;
    }

    std::vector<SDL_Rect> platforms = colliders;

    // BG tile
    SDL_Rect bg_src_rect, bg_dest_rect;
    bg_src_rect.x = 192;
    bg_src_rect.y = 128;
    bg_src_rect.w = 64;
    bg_src_rect.h = 64;
    int tile_width = 128;
    int tile_height = 128;
    bg_dest_rect.w = tile_width;
    bg_dest_rect.h = tile_height;

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
                // flip player direction if left or right arrow pressed
                if (e.type == SDL_KEYDOWN) {

                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            player_direction = LEFT;
                            break;
                        case SDLK_RIGHT:
                            player_direction = RIGHT;
                            break;
                        default:
                            break;
                    }
                }

                // handle player input
                player.handle_event(e);
            }
        }

        // process player actions after calculating time since last frame
        player.move(delta_time, colliders);
        camera.center_on_player(player);

        // Clear Screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        // Loop to tile the bg image across the screen
        for(int x = 0; x < LEVEL_WIDTH; x += tile_width) {
            for(int y = 0; y < LEVEL_HEIGHT; y += tile_height) {
                bg_dest_rect.x = x - camera.rect.x * PARALLAX_FACTOR;
                bg_dest_rect.y = y - camera.rect.y * PARALLAX_FACTOR;
                SDL_RenderCopy(renderer, bg_texture.getTexture(), &bg_src_rect, &bg_dest_rect);
            }
        }

        // Render player
        SDL_Rect dest_rect;
        int SCALE_FACTOR = 10;
        dest_rect.w = char_sprite_clips[frame / 4].w * SCALE_FACTOR;   // scale_factor > 1 for enlargement
        dest_rect.h = char_sprite_clips[frame / 4].h * SCALE_FACTOR;

        // Center the scaled sprite at the player's position
        dest_rect.x = player.get_x() - dest_rect.w / 2;
        dest_rect.y = player.get_y() - dest_rect.h / 2;
        player.render(camera.rect.x, camera.rect.y, char_sprite_sheet, renderer, full_viewport, &char_sprite_clips[frame / 4], player_direction);

        // Render platforms
        for (auto &platform : platforms) {
            SDL_Rect render_rect = platform;
            render_rect.x -= camera.rect.x;
            render_rect.y -= camera.rect.y;
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderFillRect(renderer, &render_rect);
            // TODO: randomly select section of platform texture to render

        }

        // Update Screen
        SDL_RenderPresent(renderer);

        // Determine time it took to process this frame and delay if necessary to maintain constant frame rate
        frame_end = SDL_GetTicks64();
        int frame_time = frame_end - frame_start; // time it took to process this frame

        if (frame_time < FRAME_DURATION) {
            SDL_Delay(FRAME_DURATION - frame_time); // wait the remaining frame time
        }

        ++frame;
        if (frame / 4 >= RUNNING_ANIMATION_FRAMES) {
            frame = 0;
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}