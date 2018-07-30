//
// Created by james on 7/29/18.
//

#ifndef CEF_SDL_CEF_RENDER_HANDLER_H
#define CEF_SDL_CEF_RENDER_HANDLER_H

#include "SDL.h"
#include "include/cef_render_handler.h"

class RenderHandler : public CefRenderHandler {
public:

    RenderHandler(SDL_Renderer * renderer, int w, int h);

    ~RenderHandler() override;

    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void * buffer, int w, int h) override;

    void resize(int w, int h);

    void render();

private:
    int width;
    int height;
    SDL_Renderer * renderer = nullptr;
    SDL_Texture * texture = nullptr;

    IMPLEMENT_REFCOUNTING(RenderHandler);
};


#endif //CEF_SDL_CEF_RENDER_HANDLER_H
