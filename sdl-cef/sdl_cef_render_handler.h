//
// Created by james on 7/29/18.
//

#ifndef CEF_SDL_CEF_RENDER_HANDLER_H
#define CEF_SDL_CEF_RENDER_HANDLER_H

#include "SDL.h"
#include "include/cef_render_handler.h"

class SdlCefRenderHandler : public CefRenderHandler {
public:

    /**
     *
     * @param renderer
     * @param w
     * @param h
     */
    SdlCefRenderHandler(SDL_Renderer * renderer, int w, int h);

    // destructor
    ~SdlCefRenderHandler() override;

    /**
     * // from interface
     * Called to retrieve the view rectangle which is relative to screen
     * coordinates. Return true if the rectangle was provided.
     *
     * @param browser /shrug
     * @param rect Output of function
     * @return true if successful
     */
    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

    /**
     * // from interface
     * Called when an element should be painted. Pixel values passed to this
     * method are scaled relative to view coordinates based on the value of
     * CefScreenInfo.device_scale_factor returned from GetScreenInfo. |type|
     * indicates whether the element is the view or the popup widget. |buffer|
     * contains the pixel data for the whole image. |dirtyRects| contains the set
     * of rectangles in pixel coordinates that need to be repainted. |buffer| will
     * be |width|*|height|*4 bytes in size and represents a BGRA image with an
     * upper-left origin.
     *
     * @param browser /shrug
     * @param type /shrug
     * @param dirtyRects /shrug
     * @param buffer /shrug
     * @param w /shrug
     * @param h /shrug
     */
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void * buffer, int w, int h) override;

    void resize(int w, int h);

    void render();

private:
    int width;
    int height;
    SDL_Renderer * renderer = nullptr;
    SDL_Texture * texture = nullptr;

    IMPLEMENT_REFCOUNTING(SdlCefRenderHandler);
};


#endif //CEF_SDL_CEF_RENDER_HANDLER_H
