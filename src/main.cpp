#define SDL_MAIN_HANDLED
#include <iostream>

#include "Camera.h"
#include "Game.h"
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
    TTF_Font* font = nullptr;
    if(!initialize_resources(renderer, window, font)) {
        std::cout << "Failed to initialize resources." << std::endl;
        return 1;
    }

    Player player(renderer, "Player");
    Level level(renderer);
    Camera camera;
    bool quit = false;
    while (!quit) {
        start_menu(renderer, font, quit);
        game_loop(renderer, player, level, camera, font, quit);
    }
    close(renderer, window);

    return 0;
}