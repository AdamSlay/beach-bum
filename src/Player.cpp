#include <iostream>
#include <utility>

#include "Player.h"
#include "utils.h"


const int SPAWN_X = 500;
const int SPAWN_Y = 300;
const int X_COLLIDER_OFFSET = 12;
const int Y_COLLIDER_OFFSET = 62;
const int LEVEL_HEIGHT = 800;
const int COLLIDER_EDGE_BUFFER = 6;
const float GRAVITY = 1900.0f;
const float PLAYER_SCALE = 1.2f;
const int LEFT = 1;
const int RIGHT = 0;
const std::tuple SPAWN_LOCATION = {500, 300};

const int SUSPEND_ASCEND_MAX = -125;  // First step of jumping animation (maxed out jumping)
const int SUSPEND_DESCEND_MAX = 125;  // falling faster, maxed out falling
const int SUSPEND_APEX_MIN = -50;  // apex of jump
const int SUSPEND_APEX_MAX = 50;

int JUMP_COUNT = 0;

Player::Player(SDL_Renderer* renderer, std::string animatorType) :
        width(32),
        height(48),
        velocity(400.0f),
        initial_jump_velocity(650.0f),
        pos_x(SPAWN_X),
        pos_y(SPAWN_Y),
        direction(0),
        vel_x(0.0f),
        vel_y(0.0f),
        player_collider(),
        animator(renderer, std::move(animatorType)),
        state("idle"),
        jumping(false),
        grounded(false) {
    player_collider.w  = (width * PLAYER_SCALE) - 10;  // 5 pixel buffer on each side
    player_collider.h = (height * PLAYER_SCALE) - 10;
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
                    JUMP_COUNT += 1;
                }
                // if you are in the air and have not double jumped, double jump
                else if (!grounded && JUMP_COUNT < 2) {
                    vel_y = 0;
                    jump();
                    JUMP_COUNT += 2;  // here we add 2 to prevent player from having 2 jumps if they fell off a platform
                }
                break;
            case SDLK_LEFT:
                direction = LEFT;
                break;
            case SDLK_RIGHT:
                direction = RIGHT;
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
    if(currentKeyStates[SDL_SCANCODE_LEFT]) {
        vel_x = -velocity;
    }
    else if(currentKeyStates[SDL_SCANCODE_RIGHT]) {
        vel_x = velocity;
    }
    else {
        vel_x = 0;
    }
}

void Player::move(float delta_time, std::vector<SDL_Rect>& collision_objects) {
    apply_gravity(delta_time);
    move_player_along_axis(delta_time, collision_objects);
    keep_player_on_screen();
    set_state();
}

void Player::jump() {
   vel_y -= initial_jump_velocity;
   jumping = true;
   grounded = false;
}

void Player::render(Camera& camera) {
    SDL_Rect player_rect = {pos_x, pos_y, width, height};
    camera.center_on_object(player_rect);

    int render_x = pos_x - camera.camera_rect.x;
    int render_y = pos_y - camera.camera_rect.y;
    std::tuple<int, int> render_location = {render_x, render_y};

    animator.animate(render_location, state, direction, PLAYER_SCALE);
}

int Player::get_x() const {
    return pos_x;
}

int Player::get_y() const {
    return pos_y;
}

int Player::get_width() const {
    return width;
}

int Player::get_height() const {
    return height;
}

void Player::apply_gravity(float delta_time) {
    // apply gravity
    vel_y += GRAVITY * delta_time;

    // cap vel_y at terminal velocity of 1000
    if (vel_y > 1000) {
        vel_y = 1000;
    }

    // if you're at the peak of your jump, lighten gravity a bit
    if (vel_y > 0 && vel_y < 150 && !grounded) {
        vel_y += (GRAVITY * 0.1f) * delta_time;
        vel_x *= 1.1;
    }
    else if (vel_y > 200 && !grounded) {
        vel_y += (GRAVITY * 1.1f) * delta_time;
    }
}

void Player::move_player_along_axis(float delta_time, std::vector<SDL_Rect>& collision_objects) {
    // move player along x-axis then check for collision
    pos_x += vel_x * delta_time;
    player_collider.x = pos_x + COLLIDER_EDGE_BUFFER + X_COLLIDER_OFFSET;
    for (SDL_Rect object: collision_objects) {
        if (check_collision(player_collider, object)) {
            jumping = false;
            // player is moving right and collided with object
            if (vel_x > 0) {
                player_collider.x = object.x - player_collider.w;
                pos_x = player_collider.x - COLLIDER_EDGE_BUFFER - X_COLLIDER_OFFSET;
            }
                // player is moving left and collided with object
            else if (vel_x < 0) {
                player_collider.x = object.x + object.w;
                pos_x = player_collider.x - COLLIDER_EDGE_BUFFER - X_COLLIDER_OFFSET;
            }
        }
    }
    // move player along y-axis then check for collision
    pos_y += vel_y * delta_time;
    player_collider.y = pos_y + COLLIDER_EDGE_BUFFER + Y_COLLIDER_OFFSET;
    grounded = false;
    for (SDL_Rect object: collision_objects) {

        if (check_collision_yax(player_collider, object)) {

            // player is falling and collided with object
            if (vel_y >= 0) {
                player_collider.y = object.y - player_collider.h;
                pos_y = player_collider.y - COLLIDER_EDGE_BUFFER - Y_COLLIDER_OFFSET;
                jumping = false;
                grounded = true;
                vel_y = 0;
                JUMP_COUNT = 0;
            }
                // player is jumping and collided with object
            else if (vel_y < 0) {
                player_collider.y = object.y + object.h;
                pos_y = player_collider.y - COLLIDER_EDGE_BUFFER - Y_COLLIDER_OFFSET;
                jumping = false;
                vel_y = 0;
            }
        }
    }
}

void Player::set_state() {

    // determine if running
    if (vel_x != 0 && grounded) {
        state = "running";
    }
    else if (!grounded) {
        if (vel_y >= SUSPEND_APEX_MIN && vel_y <= SUSPEND_APEX_MAX) {
            state = "suspendApex";
        }
        else if (vel_y > SUSPEND_DESCEND_MAX) {  // Suspend DESCEND Max is 200
            state = "falling";  // full speed falling
        }
        else if (vel_y < SUSPEND_ASCEND_MAX) {  // Suspend ASCEND Max is -200
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
    // keep player on screen
    if (pos_x < 0) {
        pos_x = 0;
    }
//    else if (pos_x + width > LEVEL_WIDTH) {
//        pos_x = LEVEL_WIDTH - width;
//    }
    if (pos_y < 0) {
        pos_y = 0;
    }
    else if (pos_y + height > LEVEL_HEIGHT) {
        // respawn at spawn location if you touch the bottom of the level
        pos_x = std::get<0>(SPAWN_LOCATION);
        pos_y = std::get<1>(SPAWN_LOCATION);
    }
}