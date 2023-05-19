#include <iostream>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "Player.h"
#include "Texture.h"

// Constants
const int ORIGIN_X = 0;
const int ORIGIN_Y = 0;
const int SCREEN_WIDTH = 770;
const int SCREEN_HEIGHT = 420;
const int MINIMAP_X = ORIGIN_X + 20;
const int MINIMAP_Y = ORIGIN_Y + 20;
const int MINIMAP_WIDTH = SCREEN_WIDTH / 5;
const int MINIMAP_HEIGHT = SCREEN_HEIGHT / 5;
const int FPS = 60;
const int FRAME_DURATION = 1000 / FPS;
const int RUNNING_ANIMATION_FRAMES = 8;
const int RIGHT = 0;
const int LEFT = 1;


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
    std::string bg_path = "../assets/press.png";
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
    // load minimap texture
    if (!minimap_texture.loadFromFile(minimap_path, renderer)) {
        std::cout << "Failed to load minimap_texture!" << std::endl;
        success = false;
    }
    else {
        // set minimap width and height to appropriate values
        minimap_texture.setWidth(MINIMAP_WIDTH);
        minimap_texture.setHeight(MINIMAP_HEIGHT);

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

    // Rect for full viewport
    SDL_Rect full_viewport;
    full_viewport.x = ORIGIN_X;
    full_viewport.y = ORIGIN_Y;
    full_viewport.w = SCREEN_WIDTH;
    full_viewport.h = SCREEN_HEIGHT;
    // Rect for minimap viewport
    SDL_Rect minimap_viewport;
    minimap_viewport.x = MINIMAP_X;
    minimap_viewport.y = MINIMAP_Y;
    minimap_viewport.w = MINIMAP_WIDTH;
    minimap_viewport.h = MINIMAP_HEIGHT;

    // Main loop vars
    SDL_Event e;
    Player player;
    int frame = 0;
    Uint64 frame_start = SDL_GetTicks64();
    Uint64 frame_end{};

    // block and ground rects for testing collision
    SDL_Rect block;
    block.x = 0;
    block.y = 300;
    block.w = 100;
    block.h = 100;
    SDL_Rect ground;
    ground.x = 0;
    ground.y = 400;
    ground.w = SCREEN_WIDTH;
    ground.h = 100;
    SDL_Rect platform;
    platform.x = 180;
    platform.y = 350;
    platform.w = 100;
    platform.h = 10;
    std::vector<SDL_Rect> colliders = {block, ground, platform};

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

        // Clear Screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        // Render background, char, minimap
        bg_texture.render(0,0, renderer, full_viewport, nullptr, 0);
        player.render(char_sprite_sheet, renderer, full_viewport, &char_sprite_clips[frame / 4], player_direction);
//        SDL_SetTextureAlphaMod(minimap_texture.getTexture(), 220); // set transparency for minimap
//        minimap_texture.render(0,0, renderer, minimap_viewport, nullptr, 0);

        // Render test block and ground
        SDL_RenderFillRect(renderer, &block);
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &ground);
        SDL_RenderFillRect(renderer, &platform);

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