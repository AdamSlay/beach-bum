#include <cmath>
#include <fstream>

#include <nlohmann/json.hpp>

#include "Camera.h"

Camera::Camera(){
    std::ifstream cam_config_file("../etc/camera_config.json");
    nlohmann::json camera_config;
    cam_config_file >> camera_config;

    std::ifstream run_config_file("../etc/run_config.json");
    nlohmann::json run_config;
    run_config_file >> run_config;

    origin_x = camera_config["origin_x"];
    origin_y = camera_config["origin_y"];
    SCREEN_WIDTH = run_config["SCREEN_WIDTH"];
    SCREEN_HEIGHT = run_config["SCREEN_HEIGHT"];
    GROUND_LEVEL_Y = run_config["GROUND_LEVEL_Y"];
    camera_rect = {origin_x, origin_y, SCREEN_WIDTH, SCREEN_HEIGHT};
}

float Camera::calculate_smoothing(float obj_distance, float smoothing, float range_limit, float dead_zone) {
    // if we're within the range limit, interpolate between min and max smoothing
    if (obj_distance < dead_zone) {
        return 0.0f;
    }
    else if (obj_distance < range_limit) {
        return smoothing * (obj_distance / range_limit);
    }

    // if we're outside the range limit, just return smoothing
    return smoothing;
}

void Camera::center_on_object(const SDL_Rect& object_rect, const int Y_OFFSET, const int X_OFFSET) {
    int desired_x = (object_rect.x + object_rect.w / 2) - X_OFFSET;
    int desired_y = (object_rect.y + object_rect.h / 2) - Y_OFFSET;

    // make sure camera doesn't rise above the GROUND_LEVEL_Y with some padding
    int padding = SCREEN_HEIGHT / 4;
    if (desired_y + camera_rect.h < (GROUND_LEVEL_Y + padding)) {
        desired_y = (GROUND_LEVEL_Y + padding) - camera_rect.h;

    }

    // calculate the distance to the target object
    float obj_distance_x = std::abs(object_rect.x - desired_x);
    float obj_distance_y = std::abs(object_rect.y - desired_y);

    // calculate the smoothing factor based on the distance to the edge
    float smoothing_x = calculate_smoothing(obj_distance_x, 0.05f, 200.0f, 0.0f);
    float smoothing_y = calculate_smoothing(obj_distance_y, 0.05f, 1000.0f, 0.0f);

    // apply smoothing factor to camera movement
    camera_rect.x += (desired_x - camera_rect.x) * smoothing_x;
    camera_rect.y += (desired_y - camera_rect.y) * smoothing_y;

}

void Camera::reset() {
    camera_rect = {origin_x, origin_y, SCREEN_WIDTH, SCREEN_HEIGHT};
}
