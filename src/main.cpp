#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Camera.h"
#include "Level.h"
#include "Player.h"
#include "Texture.h"

// Constants
const int ORIGIN_X = 0;
const int ORIGIN_Y = 0;
const int SCREEN_WIDTH = 770;
const int SCREEN_HEIGHT = 420;
const int LEVEL_HEIGHT = 800;
const int FPS = 60;
const int FRAME_DURATION = 1000 / FPS;


SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
std::vector<SDL_Rect> colliders;

bool initialize_resources() {
    /**
     * Initialize SDL and create window
     */

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize window
    window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cout << "Renderer could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "SDL_image could not be initialized!" << std::endl;
        std::cout << "SDL_Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
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
    if(!initialize_resources()) {
        std::cout << "Failed to initialize resources." << std::endl;
        return 1;
    }

    SDL_Event e;
    Uint64 frame_start = SDL_GetTicks64();
    Uint64 frame_end{};
    SDL_Rect camera_rect = {ORIGIN_X, ORIGIN_Y, SCREEN_WIDTH, SCREEN_HEIGHT};
    Camera camera(camera_rect, LEVEL_HEIGHT);
    Player player(renderer, "Player");
    Level level(renderer, colliders);

    bool quit = false;
    while(!quit) {
        // start frame timing
        float delta_time = static_cast<float>(SDL_GetTicks64() - frame_start) / 1000.0f;
        frame_start = SDL_GetTicks64();

        // handle player input
        while(SDL_PollEvent( &e) != 0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
            else {
                player.handle_event(e);
            }
        }

        // process player actions/movement & update level
        colliders = level.get_colliders();
        player.move(delta_time, colliders);
        level.update(player.get_x(), camera.camera_rect.x);

        // render everything
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        level.render(camera);
        player.render(camera);
        // uncomment to render player collider
//        SDL_Rect player_collider = player.get_collider();
//        SDL_Rect collider_rect = {player_collider.x - camera.camera_rect.x, player_collider.y - camera.camera_rect.y, player_collider.w, player_collider.h};
//        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Set color to red
//        SDL_RenderDrawRect(renderer, &collider_rect);

        SDL_RenderPresent(renderer);

        // Frame Timing: Determine time it took to process this frame and delay if necessary to maintain constant frame rate
        frame_end = SDL_GetTicks64();
        auto frame_time = frame_end - frame_start; // time it took to process this frame
        if (frame_time < FRAME_DURATION) {
            SDL_Delay(FRAME_DURATION - frame_time); // wait the remaining frame time
        }
    }

    close();
    return 0;
}