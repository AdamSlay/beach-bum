#ifndef SDL_PRACTICE_ANIMATOR_H
#define SDL_PRACTICE_ANIMATOR_H

#include <SDL2/SDL.h>

class Animator {

public:
    Animator();
    ~Animator();
    void animate(SDL_Renderer* renderer, SDL_Rect viewport, SDL_Rect* clip, int direction, float scale);
};


#endif //SDL_PRACTICE_ANIMATOR_H
