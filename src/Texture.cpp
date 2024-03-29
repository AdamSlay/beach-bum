#include <iostream>
#include <SDL2/SDL_image.h>
#include <memory>

#include "Texture.h"

Texture::Texture() {
    texture = nullptr;
    width = 0;
    height = 0;
}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(const std::string& path, SDL_Renderer* renderer) {
    // get rid of pre-existing texture
    free();
    SDL_Surface* loaded_surface = IMG_Load(path.c_str());

    // Load image at specified path
    if (loaded_surface == nullptr) {
        std::cout << "Unable to load image: " << path.c_str() << std::endl << "Error: " << IMG_GetError() << std::endl;
    }
    else {
        // Color key image | in this case it is set to cyan via rgb values passed to SDL_SetColorKey
        SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, 0, 0xFF, 0xFF));

        // Create texture from surface
        SDL_Texture* new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
        if (new_texture == nullptr) {
            std::cout << "Unable to create texture from " << path.c_str() << std::endl << "Error: " << SDL_GetError() << std::endl;
        }
        else {
            // Get image dimensions
            width = loaded_surface->w;
            height = loaded_surface->h;
        }

        // Free old surface
        SDL_FreeSurface(loaded_surface);

        // Create shared_ptr for new_texture and specify custom deleter
        texture = std::shared_ptr<SDL_Texture>(new_texture, SDL_DestroyTexture);
    }

    return texture != nullptr;
}

void Texture::free() {
    // free texture if it exists
    if (texture != nullptr) {
        texture = nullptr;  // shared_ptr will automatically deallocate SDL_Texture
        width = 0;
        height = 0;
    }
}

void Texture::render(std::tuple<int, int> location, SDL_Renderer* renderer, SDL_Rect* clip = nullptr, int direction = 0, float scale = 1) {
    // render the texture
    int pos_x = std::get<0>(location);
    int pos_y = std::get<1>(location);
    SDL_Rect renderQuad {pos_x, pos_y, width, height};

    if (clip != nullptr) {
        renderQuad.w = (clip->w) * scale;
        renderQuad.h = (clip->h) * scale;
    }
    // flip texture horizontally to face left
    if (direction == 1) {
        SDL_RenderCopyEx(renderer, texture.get(), clip, &renderQuad, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
    }
    else {
        SDL_RenderCopy(renderer, texture.get(), clip, &renderQuad);
    }
}

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

std::shared_ptr<SDL_Texture> Texture::getTexture() const {
    return texture;
}

void Texture::setWidth(int w) {
    width = w;
}

void Texture::setHeight(int h) {
    height = h;
}
