#define SDL_MAIN_HANDLED
#include <iostream>

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

    Camera camera;
    Player player(renderer, "Player");
    Level level(renderer);
    run_game_loop(player, level, camera, renderer);
    close(renderer, window);

    return 0;
}