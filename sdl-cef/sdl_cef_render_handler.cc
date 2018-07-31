#include "sdl_cef_render_handler.h"

SdlCefRenderHandler::SdlCefRenderHandler(SDL_Renderer *renderer, int w, int h) :
        width(w),
        height(h),
        renderer(renderer) {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
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
        unsigned char *texture_data = NULL;
        int texture_pitch = 0;

        SDL_LockTexture(texture, 0, (void **) &texture_data, &texture_pitch);
        memcpy(texture_data, buffer, w * h * 4);
        SDL_UnlockTexture(texture);
    }
}

void SdlCefRenderHandler::resize(int w, int h) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, w, h);
    width = w;
    height = h;
}

void SdlCefRenderHandler::render() {
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}
