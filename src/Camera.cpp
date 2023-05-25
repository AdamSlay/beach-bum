//
// Created by adam slay on 5/25/23.
//

#include "Camera.h"

Camera::Camera(SDL_Rect camera_rect, int width, int height) :
    rect(camera_rect),
    level_width(width),
    level_height(height) {}

void Camera::center_on_player(const Player& player) {
    int player_x = player.get_x();
    int player_y = player.get_y();
    int player_width = player.get_width();
    int player_height = player.get_height();

    int desired_x = (player_x + player_width / 2) - rect.w / 2;
    int desired_y = (player_y + player_height / 2) - rect.h * 5/7;

    float smoothing = 0.04f;

    rect.x = rect.x + (desired_x - rect.x) * smoothing;
    rect.y = rect.y + (desired_y - rect.y) * smoothing;

    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;
    if (rect.x > level_width - rect.w) rect.x = level_width - rect.w;
    if (rect.y > level_height - rect.h) rect.y = level_height - rect.h;
}