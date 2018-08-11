
#include <iostream>
#include "include/wrapper/cef_helpers.h"

#include "sdl_cef_browser_client.h"

/**
 * This class is the last step in receiving function calls from javascript (after SdlCefRenderProcessHandler and
 * SdlCefBrowserClient message router managers).
 */
class SampleMessageHandler : public CefMessageRouterBrowserSide::Handler {

public:

    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 query_id,
                 const CefString &request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override {

        // could parse "request" here for any information

        callback.get()->Success("{\"count\": " + std::to_string(++counter) + "}");

        return true;
    }

    void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int64 query_id) override {
    }

private:
    int counter = 0;
};

SdlCefBrowserClient::SdlCefBrowserClient(CefRefPtr<CefRenderHandler> ptr, const CefMessageRouterConfig &config) :
        renderHandler(ptr),
        config(config) {
}

SdlCefBrowserClient::~SdlCefBrowserClient() = default;

CefRefPtr<CefLifeSpanHandler> SdlCefBrowserClient::GetLifeSpanHandler() {
    return this;
}

CefRefPtr<CefRenderHandler> SdlCefBrowserClient::GetRenderHandler() {
    return renderHandler;
}

// CefLifeSpanHandler methods.
void SdlCefBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    browser_id = browser->GetIdentifier();

    if (!messageRouterBrowserSide) {
        // Create the browser-side router for query handling.
        messageRouterBrowserSide = CefMessageRouterBrowserSide::Create(config);

        // Register handlers with the router.
        sampleMessageHandler = new SampleMessageHandler();
        messageRouterBrowserSide->AddHandler(sampleMessageHandler, false);
    }
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
    messageRouterBrowserSide->OnBeforeClose(browser);

    CEF_REQUIRE_UI_THREAD();
    // Free the router when the last browser is closed.
    messageRouterBrowserSide->RemoveHandler(sampleMessageHandler);
    delete sampleMessageHandler;
    messageRouterBrowserSide = nullptr;
}

bool SdlCefBrowserClient::closeAllowed() const {
    return closing;
}

bool SdlCefBrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefRequest> request,
                                         bool user_gesture,
                                         bool is_redirect) {

    CEF_REQUIRE_UI_THREAD();

    messageRouterBrowserSide->OnBeforeBrowse(browser, frame);
    return false;
}

CefRefPtr<CefResourceHandler> SdlCefBrowserClient::GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) {
    CEF_REQUIRE_IO_THREAD();

//    const std::string& url = request->GetURL();

    // This is a minimal implementation of resource loading. For more complex
    // usage (multiple files, zip archives, custom handlers, etc.) you might want
    // to use CefResourceManager. See the "resource_manager" target for an
    // example implementation.
//    const std::string& resource_path = shared::GetResourcePath(url);
//    if (!resource_path.empty())
//        return shared::GetResourceHandler(resource_path);

    return nullptr;
}

void SdlCefBrowserClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                                    TerminationStatus status) {

    CEF_REQUIRE_UI_THREAD();

    messageRouterBrowserSide->OnRenderProcessTerminated(browser);
}

bool SdlCefBrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                                   CefProcessId source_process,
                                                   CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_UI_THREAD();

    return messageRouterBrowserSide->OnProcessMessageReceived(browser, source_process, message);
}

