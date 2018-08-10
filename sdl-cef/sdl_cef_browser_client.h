//
// Created by james on 7/29/18.
//

#ifndef CEF_SDL_CEF_BROWSER_CLIENT_H
#define CEF_SDL_CEF_BROWSER_CLIENT_H

#include "include/cef_render_process_handler.h"
#include "include/wrapper/cef_message_router.h"
#include "include/cef_life_span_handler.h"
#include "include/internal/cef_ptr.h"
#include "include/cef_render_handler.h"
#include "include/cef_client.h"

// for manual render handler
class SdlCefBrowserClient :
        public CefClient,
        public CefLifeSpanHandler,
        public CefRequestHandler {
public:

    explicit SdlCefBrowserClient(CefRefPtr<CefRenderHandler> ptr);

    ~SdlCefBrowserClient() override;

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

    // CefLifeSpanHandler methods.
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

    bool DoClose(CefRefPtr<CefBrowser> browser) override;

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    bool closeAllowed() const;

    // CefRequestHandler methods:
    bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefRequest> request,
                        bool user_gesture,
                        bool is_redirect) override;

    CefRefPtr<CefResourceHandler> GetResourceHandler(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request) override;

    void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                   TerminationStatus status) override;

private:
    int browser_id;
    bool closing = false;
    CefRefPtr<CefRenderHandler> renderHandler;
    CefRefPtr<CefMessageRouterBrowserSide> messageRouterBrowserSide;
    CefMessageRouterBrowserSide::Handler *sampleMessageHandler;

IMPLEMENT_REFCOUNTING(SdlCefBrowserClient);
};

#endif //CEF_SDL_CEF_BROWSER_CLIENT_H
