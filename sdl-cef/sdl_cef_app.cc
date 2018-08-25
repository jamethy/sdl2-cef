
#include "sdl_cef_app.h"

#include <iostream>
#include <thread>

/**
 * This class (through the CefApp impl) registers javascript functions with the CEF renderer.
 * When the javascript functions defined by CefMessageRouterConfig are called, it passes through here and
 * on to the SdlCefBrowserClient which manages a CefMessageRouterBrowserSide instance which sends the request
 * to a CefMessageRouterBrowserSide::Handler implementation
 */
class SdlCefRenderProcessHandler : public CefRenderProcessHandler {
public:

    explicit SdlCefRenderProcessHandler(const CefMessageRouterConfig &config) : config(config) {}

    // CefRenderProcessHandler methods
    void OnWebKitInitialized() override {
        messageRouterRenderSide = CefMessageRouterRendererSide::Create(config);
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override {
        messageRouterRenderSide->OnContextCreated(browser, frame, context);
    }

    void OnContextReleased(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefV8Context> context) override {
        messageRouterRenderSide->OnContextReleased(browser, frame, context);
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override {
        return messageRouterRenderSide->OnProcessMessageReceived(browser, source_process, message);
    }

private:

    const CefMessageRouterConfig &config;
    CefRefPtr<CefMessageRouterRendererSide> messageRouterRenderSide;

IMPLEMENT_REFCOUNTING(SdlCefRenderProcessHandler);
};

class SdlCefBrowserProcessHandler : public CefBrowserProcessHandler {

    void OnScheduleMessagePumpWork(int64 delay_ms) override {
//        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
//        CefDoMessageLoopWork();
    }

IMPLEMENT_REFCOUNTING(SdlCefBrowserProcessHandler);
};

// SdlCefApp impl
SdlCefApp::SdlCefApp(const CefMessageRouterConfig &config)
        : renderProcessHandler(new SdlCefRenderProcessHandler(config)),
        browserProcessHandler(new SdlCefBrowserProcessHandler)
        {}

void SdlCefApp::doCefWork() {
    CefDoMessageLoopWork();
}

CefRefPtr<CefRenderProcessHandler> SdlCefApp::GetRenderProcessHandler() {
    return renderProcessHandler;
}

CefRefPtr<CefBrowserProcessHandler> SdlCefApp::GetBrowserProcessHandler() {
    return browserProcessHandler;
}


