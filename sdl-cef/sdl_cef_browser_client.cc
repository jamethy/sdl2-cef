
#include <iostream>
#include "include/wrapper/cef_helpers.h"

#include "sdl_cef_browser_client.h"

SdlCefBrowserClient::SdlCefBrowserClient(CefRefPtr<CefRenderHandler> ptr) :
        handler(ptr) {
}

CefRefPtr<CefLifeSpanHandler> SdlCefBrowserClient::GetLifeSpanHandler() {
    return this;
}

CefRefPtr<CefLoadHandler> SdlCefBrowserClient::GetLoadHandler() {
    return this;
}

CefRefPtr<CefRenderHandler> SdlCefBrowserClient::GetRenderHandler() {
    return handler;
}

// CefLifeSpanHandler methods.
void SdlCefBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();

    browser_id = browser->GetIdentifier();
}

bool SdlCefBrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();

    // Closing the main window requires special handling. See the DoClose()
    // documentation in the CEF header for a detailed description of this
    // process.
    if (browser->GetIdentifier() == browser_id) {
        // Set a flag to indicate that the window close should be allowed.
        closing = true;
    }

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void SdlCefBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
}

void SdlCefBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
    std::cout << "OnLoadEnd(" << httpStatusCode << ")" << std::endl;
    loaded = true;
}

bool SdlCefBrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                      CefLoadHandler::ErrorCode errorCode, const CefString &failedUrl,
                                      CefString &errorText) {
    std::cout << "OnLoadError()" << std::endl;
    loaded = true;
    return true;
}

void SdlCefBrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack,
                                               bool canGoForward) {
    std::cout << "OnLoadingStateChange()" << std::endl;
}

void SdlCefBrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
    std::cout << "OnLoadStart()" << std::endl;
}

bool SdlCefBrowserClient::closeAllowed() const {
    return closing;
}

bool SdlCefBrowserClient::isLoaded() const {
    return loaded;
}

