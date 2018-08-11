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

/**
 * This class serves two main functions: 1) Manages the messageRouterBrowserSide instance that handles javascript calls
 * and 2) know when it the Cef window has closed
 */
class SdlCefBrowserClient :
        public CefClient,
        public CefLifeSpanHandler,
        public CefRequestHandler {
public:

    explicit SdlCefBrowserClient(CefRefPtr<CefRenderHandler> ptr, const CefMessageRouterConfig& config);

    ~SdlCefBrowserClient() override;

    bool closeAllowed() const;

    // CefClient methods
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

    // CefLifeSpanHandler methods.
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

    bool DoClose(CefRefPtr<CefBrowser> browser) override;

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

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
    const CefMessageRouterConfig& config;

IMPLEMENT_REFCOUNTING(SdlCefBrowserClient);
};

#endif //CEF_SDL_CEF_BROWSER_CLIENT_H
