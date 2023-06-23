#include <string>
#include <vector>
#include <iostream>

#include <nlohmann/json.hpp>
#include <fstream>

#include "Animator.h"

Animator::Animator(SDL_Renderer* _renderer, std::string type): renderer(_renderer), frame(-1), previous_state("none"), animations(){

    // parse json animation file
    std::ifstream configFileStream("../etc/animation_config.json");
    nlohmann::json animationConfig;
    configFileStream >> animationConfig;

    // For each character
    for (auto& [animatorType, animator] : animationConfig.items()) {
        // For each animation of the character
        if (animatorType == type) {
            for (auto& [animationName, animationValue] : animator["Animations"].items()) {
                std::string name = animationName;
                std::string file_path = animationValue["file_path"];
                int frame_count = animationValue["frame_count"];
                int frame_width = animationValue["frame_width"];
                int frame_height = animationValue["frame_height"];
                std::tuple<int, int> start_location;
                start_location = std::make_tuple(animationValue["start_position_x"], animationValue["start_position_y"]);

                auto texture = std::make_shared<Texture>();
                if (!texture->loadFromFile(file_path, renderer)) {
                    std::cout << "Failed to load texture from " << file_path << "!\n";
                    continue;
                }
                else {
                    std::cout << "Loaded texture from " << file_path << "!\n";
                }

                std::vector<SDL_Rect> frames;
                for (int i = 0; i < frame_count; ++i) {
                    SDL_Rect frame;
                    frame.x = std::get<0>(start_location) + i * frame_width;
                    frame.y = std::get<1>(start_location);
                    frame.w = frame_width;
                    frame.h = frame_height;
                    frames.push_back(frame);
                }

                animations[name] = Animation{name,
                                             file_path,
                                             frame_count,
                                             frame_width,
                                             frame_height,
                                             start_location,
                                             std::move(texture),
                                             frames};
            }
        }
    }
}

Animator::~Animator() {
}

void Animator::update_previous_state(std::string state) {
    if (previous_state != state) {
        previous_state = state;
        frame = 0;
    }
    else {
        frame++;
    }
}

void Animator::animate(std::tuple<int, int> location, std::string state, int direction, float scale) {
    update_previous_state(state);
    int anim_frame = frame / 4;  // 4 realtime frames per animation frame otherwise it's too fast
    bool state_found = false;

    std::vector<std::string> possible_states = {"running", "jumping", "falling", "idle"};
    for (std::string& possible_state : possible_states) {
        if (state == possible_state) {
            state_found = true;
            Animation& animation = animations[state];
            std::shared_ptr<Texture> texture = animation.texture;
            if (anim_frame >= animation.frame_count) {
                anim_frame = 0;
                frame = 0;
            }
            texture->render(location, renderer, &animation.frames[anim_frame], direction, scale);
            break;
        }
    }
    if (!state_found) {
        std::cout << "State " << state << " not found!\n";
    }
}
