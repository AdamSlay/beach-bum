//
// Created by adam slay on 5/25/23.
//

#include "Camera.h"

Camera::Camera(SDL_Rect camera_rect, int width, int height) :
    rect(camera_rect),
    level_width(width),
    level_height(height) {}

float Camera::calculate_smoothing(int edge_distance, float min_smoothing, float max_smoothing) {
    // range_limit is the distance to the edge of the level at which we start slowing down the camera
    float range_limit = 200.0f;

    // if we're within the range limit, interpolate between min and max smoothing
    if(edge_distance < range_limit) {
        return min_smoothing + (max_smoothing - min_smoothing) * (edge_distance / range_limit);
    }

    // if we're outside the range limit, just return max_smoothing
    return max_smoothing;
}

void Camera::center_on_player(const Player& player) {
    int player_x = player.get_x();
    int player_y = player.get_y();
    int player_width = player.get_width();
    int player_height = player.get_height();

    int desired_x = (player_x + player_width / 2) - rect.w / 2;
    int desired_y = (player_y + player_height / 2) - rect.h * 5/7;

    // calculate the distance to the edge of the level
    int edge_distance_x = std::min(rect.x, level_width - (rect.x + rect.w));
    int edge_distance_y = std::min(rect.y, level_height - (rect.y + rect.h));

    // calculate the smoothing factor based on the distance to the edge
    float smoothing_x = calculate_smoothing(edge_distance_x, 0.04f, 1.0f);
    float smoothing_y = calculate_smoothing(edge_distance_y, 0.04f, 1.0f);

    rect.x = rect.x + (desired_x - rect.x) * smoothing_x;
    rect.y = rect.y + (desired_y - rect.y) * smoothing_y;

    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;
    if (rect.x > level_width - rect.w) rect.x = level_width - rect.w;
    if (rect.y > level_height - rect.h) rect.y = level_height - rect.h;
}
