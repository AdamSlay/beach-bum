#include <iostream>

#include "Game.h"

#include "utils.h"


Game::Game(Player& player, Level& level, Camera& camera, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font)
        : player(player), level(level), camera(camera), window(window), renderer(renderer), font(font), quit(false) {
        }

Game::~Game() {
    close();
}

void Game::run() {
    while (!quit) {
        start_menu();
        game_loop();
    }
}

void game_loop() {


}


void run_loop(){


}


void start_menu(){


}


void handle_keyboard_events(SDL_Event& e){


}


void render_score(const std::string& text, SDL_Color color, int x, int y){


}

bool check_collision(SDL_Rect a, SDL_Rect b){

   return false;
}

