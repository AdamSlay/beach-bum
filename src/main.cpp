#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "Camera.h"
#include "Level.h"
#include "Player.h"
#include "Texture.h"

#include "utils.h"

// Constants
const int ORIGIN_X = 0;
const int ORIGIN_Y = 0;
const int SCREEN_WIDTH = 770;
const int SCREEN_HEIGHT = 420;
const int LEVEL_HEIGHT = 800;
const int FPS = 60;
const int FRAME_DURATION = 1000 / FPS;


int main( int argc, char* argv[] ) {
    /**
     * Beach Bum Game
     */

    //Start up SDL and create window/renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<SDL_Rect> colliders;
    if(!initialize_resources(renderer, window)) {
        std::cout << "Failed to initialize resources." << std::endl;
        return 1;
    }

    // Initialize Camera, Player, Level objects
    SDL_Rect camera_rect = {ORIGIN_X, ORIGIN_Y, SCREEN_WIDTH, SCREEN_HEIGHT};
    Camera camera(camera_rect, LEVEL_HEIGHT);
    Player player(renderer, "Player");
    Level level(renderer, colliders);

    // Main loop
    Uint64 frame_start = SDL_GetTicks64();
    Uint64 frame_end{};
    SDL_Event e;
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
//        render_player_collider(player, renderer, camera);  // uncomment to render player collider

        SDL_RenderPresent(renderer);

        // Frame Timing: Determine time it took to process this frame and delay if necessary to maintain constant frame rate
        frame_end = SDL_GetTicks64();
        auto frame_time = frame_end - frame_start; // time it took to process this frame
        if (frame_time < FRAME_DURATION) {
            SDL_Delay(FRAME_DURATION - frame_time); // wait the remaining frame time
        }
    }

    close(renderer, window);
    return 0;
}