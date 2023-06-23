#ifndef SDL_PRACTICE_ANIMATOR_H
#define SDL_PRACTICE_ANIMATOR_H

#include <SDL2/SDL.h>

class Animator {

public:
    SDL_Renderer* renderer;
    std::string previous_state;
    int frame;
    Animator(SDL_Renderer* renderer);
    ~Animator();
    void animate(std::tuple<int, int> location, std::string state, int direction, float scale);
};


#endif //SDL_PRACTICE_ANIMATOR_H
