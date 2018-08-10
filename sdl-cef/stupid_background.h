//
// Created by james on 7/29/18.
//

#ifndef STUPID_BACKGROUND_H
#define STUPID_BACKGROUND_H

#include <random>
#include <vector>
#include "SDL.h"

class StupidBackground {
public:

    StupidBackground(int count, int viewWidth, int viewHeight, SDL_Renderer *renderer);

    void render();

    void resize(int width, int height);

private:

    typedef struct BackgroundImage {
        int width;
        int x;
        int y;
        int rotation;
        int dx;
        int dy;
        int dr;
    } BackgroundImage;

    BackgroundImage createImage();
    void incrementImages();

    std::vector<BackgroundImage> images;

    int count;
    int viewWidth;
    int viewHeight;

    std::default_random_engine generator;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
};

#endif // STUPID_BACKGROUND_H
