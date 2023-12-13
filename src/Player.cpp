#include <iostream>
#include <fstream>
#include <utility>

#include <nlohmann/json.hpp>

#include "Player.h"
#include "utils.h"


PlayerConfig PlayerConfig::loadFromJson(const std::string& filePath) {
    std::ifstream configFile(filePath);
    nlohmann::json configJson;
    configFile >> configJson;

    PlayerConfig config;
    config.width = configJson["width"];
    config.height = configJson["height"];
    config.velocity = configJson["velocity"];
    config.initialJumpVelocity = configJson["initialJumpVelocity"];
    config.spawnX = configJson["spawnX"];
    config.spawnY = configJson["spawnY"];
    config.xColliderOffset = configJson["xColliderOffset"];
    config.yColliderOffset = configJson["yColliderOffset"];
    config.levelHeight = configJson["levelHeight"];
    config.colliderEdgeBuffer = configJson["colliderEdgeBuffer"];
    config.gravity = configJson["gravity"];
    config.player_scale = configJson["player_scale"];
    config.left = configJson["left"];
    config.right = configJson["right"];
    config.suspendAscendMax = configJson["suspendAscendMax"];
    config.suspendDescendMax = configJson["suspendDescendMax"];
    config.suspendApexMin = configJson["suspendApexMin"];
    config.suspendApexMax = configJson["suspendApexMax"];
    config.availableJumps = configJson["availableJumps"];
    config.dashDuration = configJson["dashDuration"];
    config.dashCooldown = configJson["dashCooldown"];
    config.dashVelocity = configJson["dashVelocity"];
    return config;
}

Player::Player(SDL_Renderer* renderer, std::string animatorType)
        : config(PlayerConfig::loadFromJson("../etc/player_config.json")),
          animator(renderer, std::move(animatorType)),
          player_collider() {
    jump_count = 0;
    pos_x = config.spawnX;
    pos_y = config.spawnY;
    player_collider.w = config.width - (config.colliderEdgeBuffer);
    player_collider.h = config.height;
    dash_time_remaining = 0;
    dash_cooldown_remaining = 0;
}

void Player::handle_event(SDL_Event& e) {
    // keyboard state
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

    // check if jump key has been pressed
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                // if you are grounded, jump
                if (!jumping && grounded) {
                    jump();
                }
                // if you are in the air and have not double jumped, double jump
                else if (!grounded && jump_count < config.availableJumps) {
                    vel_y = 0;
                    jump();
                }
                break;
                // don't do anything if you press left
//            case SDLK_LEFT:
//                direction = config.left;
//                break;
            case SDLK_RIGHT:
                direction = config.right;
                dash();
                break;
            case SDLK_SPACE:
                vel_x = config.velocity; // wait until user presses 'space' to start vel_x on a new run
                break;
            default:
                break;
        }
    }

    if(!currentKeyStates[SDL_SCANCODE_UP]) {
        // the jump button is no longer being held down
        if (jumping && vel_y < 0) {
            vel_y = 0;  // stop upward movement if jump button is released
        }
    }

    // x-axis
    // This is the player controller if you don't want a runner
//    if(currentKeyStates[SDL_SCANCODE_LEFT]) {
//        vel_x = -config.velocity;
//    }
//    else if(currentKeyStates[SDL_SCANCODE_RIGHT]) {
//        vel_x = config.velocity;
//    }
//    else {
//        vel_x = 0;
//    }
}

void Player::move(float delta_time, std::vector<SDL_Rect>& collision_objects) {
    apply_gravity(delta_time);
    move_player_along_axis(delta_time, collision_objects);

    if (dash_time_remaining > 0) {
        dash_time_remaining -= delta_time;
        vel_y = 0;
        if (dash_time_remaining <= 0) {
            vel_x -= config.dashVelocity;
            dash_cooldown_remaining = config.dashCooldown; // can't dash again until cooldown over
        }
    }
    else if (dash_cooldown_remaining > 0) {
        dash_cooldown_remaining -= delta_time;
    }

    set_state();
    keep_player_on_screen();
    if (vel_x > 0) {
        vel_x += 0.1;
    }
}

void Player::jump() {
   vel_y -= config.initialJumpVelocity;
   jumping = true;
   jump_count += 1;
}

void Player::dash() {
    if (dash_cooldown_remaining <= 0 && dash_time_remaining <= 0) {
        vel_x += config.dashVelocity;
        dash_time_remaining = config.dashDuration;
    }
}

void Player::render(Camera& camera) {
    SDL_Rect player_rect = {pos_x, pos_y, config.width, config.height};
    camera.center_on_object(player_rect);

    int render_x = pos_x - camera.camera_rect.x;
    int render_y = pos_y - camera.camera_rect.y;
    std::tuple<int, int> render_location = {render_x, render_y};

    animator.animate(render_location, state, direction, config.player_scale);
}

int Player::get_x() const {
    return pos_x;
}

int Player::get_y() const {
    return pos_y;
}

int Player::get_width() const {
    return config.width;
}

int Player::get_height() const {
    return config.height;
}

void Player::apply_gravity(float delta_time) {
    // apply gravity
    vel_y += config.gravity * delta_time;

    // cap vel_y at terminal velocity of 1000
    if (vel_y > 1000) {
        vel_y = 1000;
    }

    // if you're at the peak of your jump, lighten gravity a bit
    if (vel_y > 0 && vel_y < 150 && !grounded) {
        vel_y += (config.gravity * 0.1f) * delta_time;
    }
    else if (vel_y > 200 && !grounded) {
        vel_y += (config.gravity * 1.1f) * delta_time;
    }
}

void Player::move_player_along_axis(float delta_time, std::vector<SDL_Rect>& collision_objects) {
    // move player along x-axis then check for collision
    pos_x += vel_x * delta_time;
    player_collider.x = pos_x + config.colliderEdgeBuffer + config.xColliderOffset;
    for (SDL_Rect object: collision_objects) {
        if (check_collision(player_collider, object)) {
            jumping = false;
            // player is moving right and collided with object
            if (vel_x > 0) {
                player_collider.x = object.x - player_collider.w;
                pos_x = player_collider.x - config.colliderEdgeBuffer - config.xColliderOffset;
            }
                // player is moving left and collided with object
            else if (vel_x < 0) {
                player_collider.x = object.x + object.w;
                pos_x = player_collider.x - config.colliderEdgeBuffer - config.xColliderOffset;
            }
        }
    }
    // move player along y-axis then check for collision
    pos_y += vel_y * delta_time;
    player_collider.y = pos_y + config.colliderEdgeBuffer + config.yColliderOffset;
    grounded = false;
    for (SDL_Rect object: collision_objects) {

        if (check_collision_yax(player_collider, object)) {

            // player is falling and collided with object
            if (vel_y >= 0) {
                player_collider.y = object.y - player_collider.h;
                pos_y = player_collider.y - config.colliderEdgeBuffer - config.yColliderOffset;
                jumping = false;
                grounded = true;
                vel_y = 0;
                jump_count = 0;
            }
                // player is jumping and collided with object
            else if (vel_y < 0) {
                player_collider.y = object.y + object.h;
                pos_y = player_collider.y - config.colliderEdgeBuffer - config.yColliderOffset;
                jumping = false;
                vel_y = 0;
            }
        }
    }
}

void Player::set_state() {
    // always reset jump_count if grounded.
    if (grounded) {
        jump_count = 0;
    }
    // determine if running
    if (vel_x != 0 && grounded) {
        state = "running";
    }
    else if (!grounded) {
        if (vel_y >= config.suspendApexMin && vel_y <= config.suspendApexMax) {
            state = "suspendApex";
        }
        else if (vel_y > config.suspendDescendMax) {  // Suspend DESCEND Max is 200
            state = "falling";  // full speed falling
        }
        else if (vel_y < config.suspendAscendMax) {  // Suspend ASCEND Max is -200
            state = "jumping";  // full speed jumping
        }
        else if (vel_y > 0) {
            state = "suspendDescend";  // slow falling
        }
        else if (vel_y < 0) {
            state = "suspendAscend";  // slow jumping
        }
    }
    else {
        state = "idle";
    }
}

void Player::keep_player_on_screen() {
    // respawn at spawn location if you fall off the level
    if (pos_y + config.height > config.levelHeight) {
        pos_x = config.spawnX;
        pos_y = config.spawnY;
        vel_x = 0;
        state = "dead";
    }
}

SDL_Rect Player::get_collider() {
    return player_collider;
}