
#include "sdl_cef_render_handler.h"
#include "sdl_cef_browser_client.h"
#include "sdl_cef_app.h"
#include "sdl_cef_events.h"
#include "stupid_background.h"
#include "frame_rate_ticker.h"

#include "include/cef_browser.h"
#include "include/cef_app.h"

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>

const int INITIAL_WINDOW_WIDTH = 1000;
const int INITIAL_WINDOW_HEIGHT = 1000;

/** Shorthand to clean up objects if created */
void cleanUp(SDL_Window* window = nullptr, SDL_Renderer* renderer = nullptr, bool cefInitialized = false) {
    if (cefInitialized) {
        CefShutdown();
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

int main(int argc, char *argv[]) {

    // CEF settings and config
    CefMessageRouterConfig messageRouterConfig;
    CefRefPtr<SdlCefApp> cefApp = new SdlCefApp(messageRouterConfig);

    // This block of code is called first because CEF will call this executable
    // to start separate processes. So anything above this point would be called multiple times.
    CefMainArgs args(argc, argv);
    auto exitCode = CefExecuteProcess(args, cefApp, nullptr);
    if (exitCode >= 0) {
        return exitCode;
    }

    // Initialize Simple DirectMedia Layer
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create the SDL window to draw stuff on
    auto window = SDL_CreateWindow("Render CEF with SDL",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   INITIAL_WINDOW_WIDTH,
                                   INITIAL_WINDOW_HEIGHT,
                                   SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL could not create window! SDL_Error: " << SDL_GetError() << std::endl;
        cleanUp();
        return -1;
    }

    // Create the SDL object to render stuff with
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL could not create renderer! SDL_Error: " << SDL_GetError() << std::endl;
        cleanUp(window);
        return -1;
    }

    // Initialize Chromium Embedded Framework
    CefSettings settings;
    settings.windowless_rendering_enabled = true;
    if (!CefInitialize(args, settings, cefApp, nullptr)) {
        std::cerr << "CEF could not initialize!\n";
        cleanUp(window, renderer);
        return -1;
    }

    // create moving background image for example
    StupidBackground background(50, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, renderer);

    // Create the renderer handler - this takes the image buffer that CEF fills using the HTML and puts it
    // in an SDL texture
    CefRefPtr<SdlCefRenderHandler> renderHandler = new SdlCefRenderHandler(renderer,
                                                                           INITIAL_WINDOW_WIDTH,
                                                                           INITIAL_WINDOW_HEIGHT);

    // Create a browser client - this ties into the life cycle of the browser
    CefRefPtr<SdlCefBrowserClient> browserClient = new SdlCefBrowserClient(renderHandler, messageRouterConfig);

    // some browser settings
    CefWindowInfo window_info;
    window_info.SetAsWindowless(kNullWindowHandle);

    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = 60;
    browserSettings.background_color = 0; // allows for transparency

    // Create the browser object to interpret the HTML
    std::string htmlFile = "file://" + std::string(SDL_GetBasePath()) + "sdl_cef_html.html";
    CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info,
                                                                      browserClient,
                                                                      htmlFile,
                                                                      browserSettings,
                                                                      nullptr);

    FrameRateTicker ticker(120);
    while (!browserClient->closeAllowed()) {

        // send events to browser
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {

            // handle quit and window resize events separately
            if (e.type == SDL_QUIT) {
                browser->GetHost()->CloseBrowser(false);

            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                renderHandler->resize(e.window.data1, e.window.data2);
                background.resize(e.window.data1, e.window.data2);
                browser->GetHost()->WasResized();

            } else {
                handleEvent(e, browser.get());
            }
        }

        // let browser process events
        CefDoMessageLoopWork();

        // clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // draw the background then the HTML
        background.render();
        renderHandler->render();

        // Update screen
        SDL_RenderPresent(renderer);

        // make sure we're not going too fast
        ticker.tick();
    }

    // clean up
    browser = nullptr;
    browserClient = nullptr;
    renderHandler = nullptr;

    cleanUp(window, renderer, true);

    return 0;
}
