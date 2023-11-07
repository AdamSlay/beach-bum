#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "Camera.h"
#include "Level.h"
#include "Player.h"
#include "Texture.h"

#include "utils.h"


int main( int argc, char* argv[] ) {
    /**
     * Beach Bum Game
     */

    // Initialize SDL and create window/renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if(!initialize_resources(renderer, window)) {
        std::cout << "Failed to initialize resources." << std::endl;
        return 1;
    }

    // Camera
    Camera camera;
    // Player
    Player player(renderer, "Player");
    // Level
    std::vector<SDL_Rect> colliders;
    Level level(renderer, colliders);

    // Run the game loop
    run_game_loop(player, level, camera, renderer, colliders);

    // Free resources and close SDL
    close(renderer, window);
    return 0;
}