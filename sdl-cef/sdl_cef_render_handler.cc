#include "sdl_cef_render_handler.h"

RenderHandler::RenderHandler(SDL_Renderer *renderer, int w, int h) :
        width(w),
        height(h),
        renderer(renderer) {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
}

RenderHandler::~RenderHandler() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    renderer = nullptr;
}

bool RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
    rect = CefRect(0, 0, width, height);
    return true;
}

void RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                            const void *buffer, int w, int h) {
    if (texture) {
        unsigned char *texture_data = NULL;
        int texture_pitch = 0;

        SDL_LockTexture(texture, 0, (void **) &texture_data, &texture_pitch);
        memcpy(texture_data, buffer, w * h * 4);
        SDL_UnlockTexture(texture);
    }
}

void RenderHandler::resize(int w, int h) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
    width = w;
    height = h;
}

void RenderHandler::render() {
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}
