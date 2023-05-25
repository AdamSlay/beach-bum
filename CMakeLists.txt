cmake_minimum_required(VERSION 3.24)
project(SDL_practice)

set(CMAKE_CXX_STANDARD 17)

# Find the SDL2 package
find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED)

# Set the source files for your project
set(SOURCE_FILES
        src/main.cpp
        src/Texture.cpp src/Texture.h src/Player.cpp src/Player.h src/utils.cpp src/utils.h src/Camera.cpp src/Camera.h)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link the SDL2 framework
target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        )
