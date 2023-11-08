#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"


const int SCREEN_WIDTH = 770;
const int SCREEN_HEIGHT = 420;
const int FPS = 60;
const int FRAME_DURATION = 1000 / FPS;

void run_game_loop(SDL_Renderer* renderer, Player& player, Level& level, Camera& camera, TTF_Font* font) {
    /**
     * Beach Bum Game loop
     */

    // Main loop
    Uint64 frame_start = SDL_GetTicks64();
    Uint64 frame_end{};
    SDL_Event e;
    std::vector<SDL_Rect> colliders;

    bool quit = false;
    while(!quit) {
        // start frame timing
        float delta_time = static_cast<float>(SDL_GetTicks64() - frame_start) / 1000.0f;
        frame_start = SDL_GetTicks64();

        handle_keyboard_events(player, e, quit);

        // process player actions/movement & update level
        colliders = level.get_colliders();
        player.move(delta_time, colliders);
        level.update(player.get_x(), camera.camera_rect.x);

        // render everything
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        level.render(camera);
        player.render(camera);
        render_score(renderer, std::to_string(player.get_x() / 10), font, {255, 245, 140, 255});
//        render_player_collider(player, renderer, camera);  // uncomment to render player collider

        SDL_RenderPresent(renderer);

        // Frame Timing: Determine time it took to process this frame and delay if necessary to maintain constant frame rate
        frame_end = SDL_GetTicks64();
        auto frame_time = frame_end - frame_start; // time it took to process this frame
        if (frame_time < FRAME_DURATION) {
            SDL_Delay(FRAME_DURATION - frame_time); // wait the remaining frame time
        }
    }


}

bool initialize_resources(SDL_Renderer*& renderer, SDL_Window*& window, TTF_Font*& font) {
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

    if(TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // FONT
    font = TTF_OpenFont("../etc/fonts/SuperFunky.ttf", 24);
    if (font == nullptr) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

void handle_keyboard_events(Player& player, SDL_Event& e, bool& quit) {
    // handle player input
    while(SDL_PollEvent( &e) != 0) {
        if(e.type == SDL_QUIT) {
            quit = true;
        }
        else {
            player.handle_event(e);
        }
    }
}

void render_score(SDL_Renderer* renderer, const std::string& text, TTF_Font* font, SDL_Color color) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == nullptr) {
        std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == nullptr) {
        std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);

    SDL_Rect renderQuad = {360, 50, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

void render_player_collider(Player& player, SDL_Renderer* renderer, Camera& camera) {
    /**
     * Render player_collider rectangle
     */

    // uncomment to render player collider
        SDL_Rect player_collider = player.get_collider();
        SDL_Rect collider_rect = {player_collider.x - camera.camera_rect.x, player_collider.y - camera.camera_rect.y, player_collider.w, player_collider.h};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Set color to red
        SDL_RenderDrawRect(renderer, &collider_rect);
}

void close(SDL_Renderer* renderer, SDL_Window* window) {
    /**
     * Free renderer and window resources, quit SDL2 and SDL2_image
     */

    //Destroy window/renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;

    //Quit SDL/IMG subsystems
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool check_collision( SDL_Rect a, SDL_Rect b ) {
    /**
     * Check for collision between two rectangles on all sides
     */

    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of camera_rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of camera_rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside B
    if( bottomA <= topB ) {
        return false;
    }

    if( topA >= bottomB ) {
        return false;
    }

    if( rightA <= leftB ) {
        return false;
    }

    if( leftA >= rightB ) {
        return false;
    }

    return true;
}

bool check_collision_yax(SDL_Rect a, SDL_Rect b)
{
    /**
     * check_collision() with a 1 pixel buffer on the bottom of 'a'.
     * This helps the player know when it is touching the ground and
     * prevents the player player_collider from getting stuck when moving along the x-axis.
     */
    const int PIXEL_BUFFER = 1;

    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of camera_rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of camera_rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside B
    if( bottomA + PIXEL_BUFFER <= topB ) {
        return false;
    }

    if( topA >= bottomB ) {
        return false;
    }

    if( rightA <= leftB ) {
        return false;
    }

    if( leftA >= rightB ) {
        return false;
    }

    return true;
}
