
#include <iostream>
#include "sdl_cef_app.h"

CefRefPtr<CefRenderProcessHandler> SdlCefApp::GetRenderProcessHandler() {
    return this;
}

void SdlCefApp::OnWebKitInitialized() {
// Create the renderer-side router for query handling.
    CefMessageRouterConfig config;
    messageRouterRenderSide = CefMessageRouterRendererSide::Create(config);
}

void SdlCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) {
    messageRouterRenderSide->OnContextCreated(browser, frame, context);
}

void SdlCefApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) {
    messageRouterRenderSide->OnContextReleased(browser, frame, context);
}

bool SdlCefApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                         CefProcessId source_process,
                                         CefRefPtr<CefProcessMessage> message) {
    return messageRouterRenderSide->OnProcessMessageReceived(browser, source_process, message);
}
