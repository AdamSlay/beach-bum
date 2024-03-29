#include <string>
#include <memory>

#include <SDL2/SDL.h>

#ifndef SDL_PRACTICE_TEXTURE_H
#define SDL_PRACTICE_TEXTURE_H

class Texture {
public:
    explicit Texture();
    ~Texture();
    bool loadFromFile(const std::string& path, SDL_Renderer* renderer);
    void free();
    void render(std::tuple<int, int> location, SDL_Renderer* renderer, SDL_Rect* clip, int direction, float scale);
    int getWidth() const;
    int getHeight() const;
    std::shared_ptr<SDL_Texture> getTexture() const;
    void setWidth(int w);
    void setHeight(int h);
private:
    std::shared_ptr<SDL_Texture> texture;
    // image dimensions
    int width;
    int height;
};

#endif //SDL_PRACTICE_TEXTURE_H
