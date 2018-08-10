#include <iostream>
#include <fstream>
#include "sdl_cef_render_handler.h"

SdlCefRenderHandler::SdlCefRenderHandler(SDL_Renderer *renderer, int w, int h) :
        width(w),
        height(h),
        renderer(renderer) {

    resize(w, h);
}

SdlCefRenderHandler::~SdlCefRenderHandler() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    renderer = nullptr;
}

bool SdlCefRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
    rect = CefRect(0, 0, width, height);
    return true;
}

void SdlCefRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                                  const void *buffer, int w, int h) {
    if (texture) {

        unsigned char *texture_data = nullptr;
        int texture_pitch = 0;
        size_t bufferSize = static_cast<size_t>(w) * static_cast<size_t>(h) * 4;

        SDL_LockTexture(texture, nullptr, (void **) &texture_data, &texture_pitch);
        memcpy(texture_data, buffer, bufferSize);
        SDL_UnlockTexture(texture);
    }
}

void SdlCefRenderHandler::resize(int w, int h) {

    if (texture) {
        SDL_DestroyTexture(texture);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    width = w;
    height = h;
}


void SdlCefRenderHandler::render() {
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}
