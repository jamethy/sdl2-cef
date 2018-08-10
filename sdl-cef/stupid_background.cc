
#include "stupid_background.h"

#include <SDL_image.h>

StupidBackground::StupidBackground(int count, int viewWidth, int viewHeight, SDL_Renderer *renderer)
        : count(count),
          viewWidth(viewWidth),
          viewHeight(viewHeight),
          renderer(renderer) {

    std::string imgPath = std::string(SDL_GetBasePath()) + "sdl_cef_img.png";
    texture = IMG_LoadTexture(renderer, imgPath.c_str());
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    images.clear();
    for (int i = 0; i < count; ++i) {
        images.push_back(createImage());
    }

    // increment images so they start spread out
    for (int i = 0; i < 250; ++i) {
        incrementImages();
    }
}

StupidBackground::BackgroundImage StupidBackground::createImage() {

    const int minSpeed = 1;
    const int maxSpeed = 2;
    const int maxRotation = 2;

    BackgroundImage image;
    std::uniform_int_distribution<int> overSides(1, 4);
    std::uniform_int_distribution<int> overWidth(0, viewWidth);
    std::uniform_int_distribution<int> overHeight(0, viewHeight);
    std::uniform_int_distribution<int> overSize(10, viewWidth / 5);
    std::uniform_int_distribution<int> overAngle(0, 360);
    std::uniform_int_distribution<int> overDx(-maxSpeed, maxSpeed);
    std::uniform_int_distribution<int> overDxPos(minSpeed, maxSpeed);
    std::uniform_int_distribution<int> overDr(-maxRotation, maxRotation);

    image.width = overSize(generator);
    image.rotation = overAngle(generator);
    image.dr = overDr(generator);

    int side = overSides(generator);
    switch (side) {
        case 1: // top
            image.y = -image.width;
            image.x = overWidth(generator);
            image.dx = overDx(generator);
            image.dy = overDxPos(generator);
            break;
        case 2: // left
            image.x = -image.width;
            image.y = overHeight(generator);
            image.dx = overDxPos(generator);
            image.dy = overDx(generator);
            break;
        case 3: // bottom
            image.y = viewHeight + image.width;
            image.x = overWidth(generator);
            image.dx = overDx(generator);
            image.dy = -overDxPos(generator);
            break;
        default: // right
            image.x = viewWidth + image.width;
            image.y = overHeight(generator);
            image.dx = -overDxPos(generator);
            image.dy = overDx(generator);
            break;

    }

    return image;
}

void StupidBackground::incrementImages() {

    // increment images
    for (int i = 0; i < count; ++i) {
        auto &image = images[i];
        image.x += image.dx;
        image.y += image.dy;
        image.rotation += image.dr;

        if (image.x + image.width <= 0
            || image.x - image.width >= viewWidth
            || image.y + image.width <= 0
            || image.y - image.width >= viewHeight) {
            images[i] = createImage();
        }
    }
}

void StupidBackground::render() {

    for (BackgroundImage image : images) {

        SDL_Rect rect;
        rect.w = image.width;
        rect.h = rect.w;
        rect.x = image.x;
        rect.y = image.y;
        int angle = image.rotation;

        SDL_RenderCopyEx(renderer, texture,
                         nullptr, &rect,
                         angle,
                         nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
    }

    incrementImages();
}

void StupidBackground::resize(int width, int height) {
    viewHeight = height;
    viewWidth = width;
}
