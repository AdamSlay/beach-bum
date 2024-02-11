#ifndef SDL_PRACTICE_GAME_H
#define SDL_PRACTICE_GAME_H


#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "Player.h"
#include "Camera.h"
#include "Level.h"


class Game {
public:
    Game(Player& player, Level& level, Camera& camera, SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);
    ~Game();
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
};


#endif //SDL_PRACTICE_GAME_H
