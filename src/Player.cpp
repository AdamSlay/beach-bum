#include <iostream>

#include "Player.h"
#include "utils.h"


const int SCREEN_HEIGHT = 420;
const int SCREEN_WIDTH = 770;
const int COLLIDER_EDGE_BUFFER = 5;
const int GRAVITY = 1000;
const int JUMP_VELOCITY = 500;

Player::Player() : pos_x(0), pos_y(0), vel_x(0.0f), vel_y(0.0f), collider(), jumping(false), grounded(false) {
    collider.w  = width - 10;  // 5 pixel buffer on each side
    collider.h = height - 10;
}

void Player::handle_event(SDL_Event& e) {
    // keyboard state
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

    // y-axis
    if(currentKeyStates[SDL_SCANCODE_UP]) {
        jump();
    }
    else if(currentKeyStates[SDL_SCANCODE_DOWN]) {
        vel_y = velocity;
    }
    else {
        vel_y = 0;
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
    // apply gravity if not grounded
    if (!grounded) {
        vel_y += GRAVITY * delta_time;
    }

    // move player along x-axis then check for collision
    pos_x += vel_x * delta_time;
    collider.x = pos_x + COLLIDER_EDGE_BUFFER;
    for (SDL_Rect object: objects) {
        if (check_collision(collider, object)) {
            if (vel_x > 0) {
                collider.x = object.x - collider.w;
                pos_x = collider.x - COLLIDER_EDGE_BUFFER;
            }
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
        if (check_collision(collider, object)) {
            if (vel_y >= 0) {
                collider.y = object.y - collider.h;
                pos_y = collider.y - COLLIDER_EDGE_BUFFER;
                jumping = false;
                grounded = true;
                vel_y = 0;
            }
            else if (vel_y < 0) {
                collider.y = object.y + object.h;
                pos_y = collider.y - COLLIDER_EDGE_BUFFER;
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
        pos_y = SCREEN_HEIGHT - height;
    }
}

void Player::jump() {
   if (!jumping) {
       vel_y -= JUMP_VELOCITY;
       jumping = true;
       grounded = false;
   }
}

void Player::render(Texture& texture, SDL_Renderer* renderer, SDL_Rect viewport, SDL_Rect* clip = nullptr, int direction = 0) const {
    texture.render(pos_x, pos_y, renderer, viewport, clip, direction);
}