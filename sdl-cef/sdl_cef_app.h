//
// Created by james on 7/29/18.
//

#ifndef CEF_SDL_CEF_APP_H
#define CEF_SDL_CEF_APP_H

#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"

class SdlCefApp : public CefApp,
                  public CefRenderProcessHandler {
public:

    // CefApp methods:
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    // CefRenderProcessHandler methods
    void OnWebKitInitialized() override;

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override;

    void OnContextReleased(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefV8Context> context) override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

private:

    CefRefPtr<CefMessageRouterRendererSide> messageRouterRenderSide;

    IMPLEMENT_REFCOUNTING(SdlCefApp);
};

#endif //CEF_SDL_CEF_APP_H
