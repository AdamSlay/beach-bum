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

    Player player(renderer, "Player");
    Level level(renderer);
    Camera camera;
    run_game_loop(renderer, player, level, camera);
    close(renderer, window);

    return 0;
}