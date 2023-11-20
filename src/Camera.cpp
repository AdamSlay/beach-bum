#include <algorithm>
#include <cmath>
#include <fstream>

#include <nlohmann/json.hpp>

#include "Camera.h"

Camera::Camera(){
    std::ifstream config_file("../etc/camera_config.json");
    nlohmann::json config;
    config_file >> config;
    origin_x = config["origin_x"];
    origin_y = config["origin_y"];
    screen_width = config["screen_width"];
    screen_height = config["screen_height"];
    camera_rect = {origin_x, origin_y, screen_width, screen_height};
}

float Camera::calculate_smoothing(int obj_distance, float min_smoothing, float max_smoothing, float range_limit) {
    // if we're within the range limit, interpolate between min and max smoothing
    if(obj_distance < range_limit) {
        return min_smoothing + (max_smoothing - min_smoothing) * (obj_distance / range_limit);
    }

    // if we're outside the range limit, just return max_smoothing
    return max_smoothing;
}

void Camera::center_on_object(const SDL_Rect& object_rect) {
    int desired_x = (object_rect.x + object_rect.w / 2) - 100;
    int desired_y = object_rect.y + object_rect.h / 2 - camera_rect.h / 2.5;

    // calculate the distance to the edge of the level
//    int edge_distance_x = std::min(camera_rect.x, (camera_rect.x + camera_rect.w));
//    int edge_distance_y = std::min(camera_rect.y, (camera_rect.h + 400) - (camera_rect.y + camera_rect.h));
    int obj_distance_x = std::abs(object_rect.x - camera_rect.x);
    int obj_distance_y = std::abs(object_rect.y - camera_rect.y);

    // calculate the smoothing factor based on the distance to the edge
    float smoothing_x = calculate_smoothing(obj_distance_x, 0.01f, 0.05f, 200.0f);
    float smoothing_y = calculate_smoothing(obj_distance_y, 0.01f, 0.05f, 400.0f);

    // apply smoothing factor to camera movement
    camera_rect.x += (desired_x - camera_rect.x) * smoothing_x;
    camera_rect.y += (desired_y - camera_rect.y) * smoothing_y;

     if (camera_rect.x < 0) {
         camera_rect.x = 0;
     }

    if (camera_rect.y < 170) {
        camera_rect.y = 170;
    }

}

void Camera::reset() {
    camera_rect = {origin_x, origin_y, screen_width, screen_height};
}
