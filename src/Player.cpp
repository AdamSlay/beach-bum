#include <iostream>

#include "Player.h"
#include "utils.h"


const int SCREEN_HEIGHT = 800;
const int SCREEN_WIDTH = 1000;
const int COLLIDER_EDGE_BUFFER = 5;
const int GRAVITY = 1900;
const float PLAYER_SCALE = 1.2f;
const std::tuple SPAWN_LOCATION = {500, 300};

int JUMP_COUNT = 0;

Player::Player() :
        width(32),
        height(48),
        velocity(400.0f),
        initial_jump_velocity(750.0f),
        pos_x(500),
        pos_y(300),
        vel_x(0.0f),
        vel_y(0.0f),
        collider(),
        jumping(false),
        grounded(false) {
    collider.w  = (width * PLAYER_SCALE) - 10;  // 5 pixel buffer on each side
    collider.h = (height * PLAYER_SCALE) - 10;
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

void Player::move(float delta_time, std::vector<SDL_Rect>& objects) {
    // apply gravity
    vel_y += GRAVITY * delta_time;

    // cap vel_y at terminal velocity of 1000
    if (vel_y > 1000) {
        vel_y = 1000;
    }

    // if you're at the peak of your jump, lighten gravity a bit
    if (vel_y > 0 && vel_y < 150 && !grounded) {
        vel_y += (GRAVITY * 0.1) * delta_time;
        vel_x *= 1.1;
    }
    else if (vel_y > 200 && !grounded) {
        vel_y += (GRAVITY * 1.1) * delta_time;
    }

    // move player along x-axis then check for collision
    pos_x += vel_x * delta_time;
    collider.x = pos_x + COLLIDER_EDGE_BUFFER;
    for (SDL_Rect object: objects) {
        if (check_collision(collider, object)) {
            jumping = false;
            // player is moving right and collided with object
            if (vel_x > 0) {
                collider.x = object.x - collider.w;
                pos_x = collider.x - COLLIDER_EDGE_BUFFER;
            }
            // player is moving left and collided with object
            else if (vel_x < 0) {
                collider.x = object.x + object.w;
                pos_x = collider.x - COLLIDER_EDGE_BUFFER;
            }
        }
    }

    // move player along y-axis then check for collision
    pos_y += vel_y * delta_time;
    collider.y = pos_y + COLLIDER_EDGE_BUFFER;
    grounded = false;
    for (SDL_Rect object: objects) {

        if (check_collision_yax(collider, object)) {

            // player is falling and collided with object
            if (vel_y >= 0) {
                collider.y = object.y - collider.h;
                pos_y = collider.y - COLLIDER_EDGE_BUFFER;
                jumping = false;
                grounded = true;
                vel_y = 0;
                JUMP_COUNT = 0;
            }
            // player is jumping and collided with object
            else if (vel_y < 0) {
                collider.y = object.y + object.h;
                pos_y = collider.y - COLLIDER_EDGE_BUFFER;
                jumping = false;
                vel_y = 0;
            }
        }
    }

    // keep player on screen
    if (pos_x < 0) {
        pos_x = 0;
    }
    else if (pos_x + width > SCREEN_WIDTH) {
        pos_x = SCREEN_WIDTH - width;
    }
    if (pos_y < 0) {
        pos_y = 0;
    }
    else if (pos_y + height > SCREEN_HEIGHT) {
        // respawn at spawn location if you touch the bottom of the level
        pos_x = std::get<0>(SPAWN_LOCATION);
        pos_y = std::get<1>(SPAWN_LOCATION);
    }
}

void Player::jump() {
   vel_y -= initial_jump_velocity;
   jumping = true;
   grounded = false;
}

void Player::render(int camera_x, int camera_y,Texture& texture, SDL_Renderer* renderer, SDL_Rect viewport, SDL_Rect* clip = nullptr, int direction = 0) const {
    int render_x = pos_x - camera_x;
    int render_y = pos_y - camera_y;
    texture.render(render_x, render_y, renderer, viewport, clip, direction);
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