#pragma once

#include <vector>

#include <SDL2/SDL.h>

#include "Animator.h"
#include "Camera.h"
#include "Texture.h"

struct PlayerConfig {
    int width;
    int height;
    float velocity;
    float initialJumpVelocity;
    int spawnX;
    int spawnY;
    int xColliderOffset;
    int yColliderOffset;
    int levelHeight;
    int colliderEdgeBuffer;
    float gravity;
    float player_scale;
    int left;
    int right;
    int suspendAscendMax;
    int suspendDescendMax;
    int suspendApexMin;
    int suspendApexMax;
    int availableJumps;
    float dashDuration;
    float dashCooldown;

    static PlayerConfig loadFromJson(const std::string& filePath);
};

class Player {
public:
    std::string state;
    explicit Player(SDL_Renderer* renderer, std::string animatorType);
    void handle_event(SDL_Event& e);
    void move(float delta_time, std::vector<SDL_Rect>& collision_objects);
    void render(Camera& camera);
    void jump();
    void dash();
    int get_x() const;
    int get_y() const;
    int get_width() const;
    int get_height() const;
    SDL_Rect get_collider();

private:
    PlayerConfig config;
    int jump_count;
    bool jumping;
    bool grounded;
    int pos_x;
    int pos_y;
    int direction;
    float vel_x;
    float vel_y;
    float dash_time_remaining;
    float dash_cooldown_remaining;
    SDL_Rect player_collider;
    Animator animator;

    void apply_gravity(float delta_time);
    void move_player_along_axis(float delta_time, std::vector<SDL_Rect>& collision_objects);
    void set_state();
    void keep_player_on_screen();
};