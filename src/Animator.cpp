#include <string>
#include <vector>
#include <iostream>

#include "Animator.h"
#include "Texture.h"

const int PLAYER_SPRITE_WIDTH = 32;
const int PLAYER_SPRITE_HEIGHT = 48;
const int RUNNING_ANIMATION_FRAMES = 8;

struct Animation {
    const std::string name;
    const std::string file_path;
    const int frame_count;
    const Texture texture;
    const std::vector<SDL_Rect> frames;
};

Texture char_sprite_sheet_run;
Texture char_sprite_sheet_idle;
Texture char_sprite_sheet_jump;
Texture char_sprite_sheet_fall;
SDL_Rect char_run_anim_clips[8];
SDL_Rect char_jump_anim_clips[1];
SDL_Rect char_fall_anim_clips[1];
std::string char_run_path = "../assets/bb_run_sheet.png";
std::string char_jump_path = "../assets/bb_jump_sheet.png";
std::string char_fall_path = "../assets/bb_jump_sheet.png";
std::string char_idle_path = "../assets/bb.png";

Animator::Animator(SDL_Renderer* renderer): renderer(renderer), frame(0), previous_state("none"){
    // load character texture
    if (!char_sprite_sheet_run.loadFromFile(char_run_path, renderer)) {
        std::cout << "Failed to load char_sprite_sheet_run!" << std::endl;
    }
        // create char sprite clips
    else {
        for (int i = 0; i < 8; i++) {
            char_run_anim_clips[i].x = (i * 64) + 12;
            char_run_anim_clips[i].y = 52;
            char_run_anim_clips[i].w = PLAYER_SPRITE_WIDTH;
            char_run_anim_clips[i].h = PLAYER_SPRITE_HEIGHT;
        }
    }
    // load character jump texture
    if (!char_sprite_sheet_jump.loadFromFile(char_jump_path, renderer)) {
        std::cout << "Failed to load char_sprite_sheet_jump!" << std::endl;
    }
        // create char sprite clips
    else {
        char_jump_anim_clips[0].x = (2 * 64) + 12;
        char_jump_anim_clips[0].y = 40;
        char_jump_anim_clips[0].w = PLAYER_SPRITE_WIDTH;
        char_jump_anim_clips[0].h = PLAYER_SPRITE_HEIGHT + 18;
    }
    // load character fall texture
    if (!char_sprite_sheet_fall.loadFromFile(char_fall_path, renderer)) {
        std::cout << "Failed to load char_sprite_sheet_fall!" << std::endl;
    }
        // create char sprite clips
    else {
        char_fall_anim_clips[0].x = (6 * 64) + 12;
        char_fall_anim_clips[0].y = 40;
        char_fall_anim_clips[0].w = PLAYER_SPRITE_WIDTH;
        char_fall_anim_clips[0].h = PLAYER_SPRITE_HEIGHT + 18;
    }
    // load character idle texture
    if(!char_sprite_sheet_idle.loadFromFile(char_idle_path, renderer)) {
        std::cout << "Failed to load char_sprite_sheet_idle!" << std::endl;
    }

}

Animator::~Animator() {
    char_sprite_sheet_idle.free();
    char_sprite_sheet_run.free();
    char_sprite_sheet_jump.free();
    char_sprite_sheet_fall.free();
}

void Animator::animate(std::tuple<int, int> location, std::string state, int direction, float scale) {
    int anim_frame = frame / 4;  // 4 realtime frames per animation frame otherwise it's too fast
    if (previous_state != state) {
        previous_state = state;
        frame = 0;
    }
    else {
        frame++;
    }
    if (frame / 4 >= 8) {
        frame = 0;
    }

    if (state == "running") {
        char_sprite_sheet_run.render(location, renderer, &char_run_anim_clips[anim_frame], direction, scale);
    }
    else if (state == "jumping") {
        char_sprite_sheet_jump.render(location, renderer, &char_jump_anim_clips[0], direction, scale);
    }
    else if (state == "falling") {
        char_sprite_sheet_fall.render(location, renderer, &char_fall_anim_clips[0], direction, scale);
    }
    else {
        char_sprite_sheet_idle.render(location, renderer, &char_run_anim_clips[0], direction, scale);
    }
}
