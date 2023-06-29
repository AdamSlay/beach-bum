#include <ctime>
#include <iostream>
#include <random>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "Camera.h"
#include "Level.h"
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


SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
std::vector<SDL_Rect> colliders;

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

void close() {
    // Free texture resources

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

    SDL_Event e;
    Uint64 frame_start = SDL_GetTicks64();
    Uint64 frame_end{};
    SDL_Rect camera_rect = {ORIGIN_X, ORIGIN_Y, SCREEN_WIDTH, SCREEN_HEIGHT};
    Camera camera(camera_rect, LEVEL_WIDTH, LEVEL_HEIGHT);
    Player player(renderer, "Player");
    Level level(renderer, colliders);

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
        colliders = level.get_colliders();
        player.move(delta_time, colliders);

        /**
         * The block below is all of the Level component rendering
         */
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        // TODO: level.render(camera);
        level.render(camera);
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