//
// Created by james on 7/29/18.
//

#ifndef CEF_SDL_CEF_APP_H
#define CEF_SDL_CEF_APP_H

#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"

/**
 * For simply rendering HTML this class and the {@code CefRenderProcessHandler} implementation are unnecessary, but inserting
 * them into the application allows for creation of functions and values in javascript.
 *
 * By default the functions are cefQuery and cefQueryCancel, see {@code CefMessageRouterConfig}
 *
 * The {@code CefApp} implementation is only there to provide access to the {@code CefRenderProcessHandler} implementation
 */
class SdlCefApp : public CefApp {
public:

    explicit SdlCefApp(const CefMessageRouterConfig& config);

    void doCefWork();

    // CefApp methods:
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

private:
    CefRefPtr<CefRenderProcessHandler> renderProcessHandler;
    CefRefPtr<CefBrowserProcessHandler> browserProcessHandler;
    IMPLEMENT_REFCOUNTING(SdlCefApp);
};

#endif //CEF_SDL_CEF_APP_H
