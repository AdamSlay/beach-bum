//
// Created by adam slay on 5/9/23.
//
#include <string>

#include <SDL2/SDL.h>

#ifndef SDL_PRACTICE_TEXTURE_H
#define SDL_PRACTICE_TEXTURE_H


class Texture {
public:
    explicit Texture();
    ~Texture();
    bool loadFromFile(const std::string& path, SDL_Renderer* renderer);
    void free();
    void render(int pos_x, int pos_y, SDL_Renderer* renderer, SDL_Rect viewport, SDL_Rect* clip, int direction, float scale);
    int getWidth() const;
    int getHeight() const;
    SDL_Texture* getTexture() const;
    void setWidth(int w);
    void setHeight(int h);
private:
    SDL_Texture* texture;
    // image dimensions
    int width;
    int height;
};


#endif //SDL_PRACTICE_TEXTURE_H
