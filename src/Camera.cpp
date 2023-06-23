#include <algorithm>

#include "Camera.h"

Camera::Camera(SDL_Rect camera_rect, int width, int height) :
        camera_rect(camera_rect),
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

// TODO: make this center_on_object and pass in a GameObject or SDL_Rect or something
void Camera::center_on_object(const SDL_Rect& object_rect) {
    int desired_x = (object_rect.x + object_rect.w / 2) - camera_rect.w / 2;
    int desired_y = (object_rect.y + object_rect.h / 2) - camera_rect.h * 5 / 7;

    // calculate the distance to the edge of the level
    int edge_distance_x = std::min(camera_rect.x, level_width - (camera_rect.x + camera_rect.w));
    int edge_distance_y = std::min(camera_rect.y, level_height - (camera_rect.y + camera_rect.h));

    // calculate the smoothing factor based on the distance to the edge
    float smoothing_x = calculate_smoothing(edge_distance_x, 0.04f, 0.5f);
    float smoothing_y = calculate_smoothing(edge_distance_y, 0.04f, 1.0f);

    camera_rect.x = camera_rect.x + (desired_x - camera_rect.x) * smoothing_x;
    camera_rect.y = camera_rect.y + (desired_y - camera_rect.y) * smoothing_y;

    if (camera_rect.x < 0) {
        camera_rect.x = 0;
    }

    if (camera_rect.y < 0) {
        camera_rect.y = 0;
    }

    if (camera_rect.x > level_width - camera_rect.w) {
        camera_rect.x = level_width - camera_rect.w;
    }

    if (camera_rect.y > level_height - camera_rect.h) {
        camera_rect.y = level_height - camera_rect.h;
    }
}
