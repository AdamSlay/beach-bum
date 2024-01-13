//
// Created by adam slay on 1/8/24.
//
#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "Player.h"
#include "Camera.h"
#include "Level.h"


#ifndef SDL_PRACTICE_GAME_H
#define SDL_PRACTICE_GAME_H


class Game {
public:
    Game();
    ~Game();
    bool initialize();
    void run();
    void close();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    Player player;
    Level level;
    Camera camera;
    bool quit;

    void game_loop();
    void run_loop();
    void start_menu();
    void handle_keyboard_events(SDL_Event& e);
    void render_score(const std::string& text, SDL_Color color, int x, int y);
    bool check_collision(SDL_Rect a, SDL_Rect b);
    // ... any other methods you need ...
};


#endif //SDL_PRACTICE_GAME_H
