#ifndef SDL_PRACTICE_ANIMATOR_H
#define SDL_PRACTICE_ANIMATOR_H

#include <map>

#include <SDL2/SDL.h>

#include "Texture.h"

struct Animation {
    std::string name;
    std::string file_path;
    int frame_count;
    int frame_width;
    int frame_height;
    std::tuple<int, int> start_location;
    std::shared_ptr<Texture> texture;
    std::vector<SDL_Rect> frames;
};


class Animator {

public:
    SDL_Renderer* renderer;
    std::string previous_state;
    std::map<std::string, Animation> animations;
    int frame;
    Animator(SDL_Renderer* renderer, std::string animatorType);
    ~Animator();
    void update_previous_state(std::string state);
    void animate(std::tuple<int, int> location, std::string state, int direction, float scale);
};


#endif //SDL_PRACTICE_ANIMATOR_H
