
#include "sdl_cef_render_handler.h"
#include "sdl_cef_browser_client.h"
#include "sdl_cef_app.h"
#include "sdl_cef_events.h"
#include "stupid_background.h"

#include "include/cef_browser.h"
#include "include/cef_app.h"

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>
#include <chrono>
#include <thread>

using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

const long MS_PER_FRAME = 1000 / 60;
const int INITIAL_WINDOW_WIDTH = 1000;
const int INITIAL_WINDOW_HEIGHT = 2000;

int main(int argc, char *argv[]) {

    // CEF settings and config
    CefSettings settings;
    settings.windowless_rendering_enabled = true;
    CefMessageRouterConfig messageRouterConfig;

    CefRefPtr<SdlCefApp> cefApp = new SdlCefApp(messageRouterConfig);

    // This block of code is called first because CEF will call this executable
    // to start separate processes. So anything above this point would be called multiple times.
    CefMainArgs args(argc, argv);
    int exitCode = CefExecuteProcess(args, cefApp, nullptr);
    if (exitCode >= 0) {
        return exitCode;
    }

    // Initialize Chromium Embedded Framework
    if (!CefInitialize(args, settings, cefApp, nullptr)) {
        std::cerr << "CEF could not initialize!\n";
        return -1;
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

    if (window) {

        // Create the SDL object to render stuff with
        auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (renderer) {

            SDL_Event e;

            StupidBackground background(50, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, renderer);

            // Create the renderer handler - this takes the image buffer that CEF fills using the HTML and puts it
            // in an SDL texture
            CefRefPtr<SdlCefRenderHandler> renderHandler = new SdlCefRenderHandler(renderer,
                                                                                   INITIAL_WINDOW_WIDTH,
                                                                                   INITIAL_WINDOW_HEIGHT);

            // Create a browser client - this ties into the life cycle of the browser
            CefRefPtr<SdlCefBrowserClient> browserClient = new SdlCefBrowserClient(renderHandler);

            CefWindowInfo window_info;
            window_info.SetAsWindowless(kNullWindowHandle);
            window_info.windowless_rendering_enabled = true;

            CefBrowserSettings browserSettings;
            browserSettings.background_color = 0;

//            std::string htmlFile = "file://" + std::string(SDL_GetBasePath()) + "sdl_cef_html.html";
            std::string htmlFile = "http://google.com";
            CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient,
                                                                              htmlFile,
                                                                              browserSettings, nullptr);

            bool shutdown = false;
            while (!browserClient->closeAllowed()) {

                auto start = steady_clock::now();

                // send events to browser
                while (!shutdown && SDL_PollEvent(&e) != 0) {
                    shutdown = handleEvent(e, browser.get());

                    // handle the window resize event separately
                    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        renderHandler->resize(e.window.data1, e.window.data2);
                        background.resize(e.window.data1, e.window.data2);
                        browser->GetHost()->WasResized();
                    }
                }

                // let browser process events
                CefDoMessageLoopWork();

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                // render
                SDL_RenderClear(renderer);

                background.render();
                renderHandler->render();

                // Update screen
                SDL_RenderPresent(renderer);

                auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
                if (elapsed < MS_PER_FRAME) {
                    std::this_thread::sleep_for(milliseconds(MS_PER_FRAME - elapsed));
                }
            }

            // clean up
            browser = nullptr;
            browserClient = nullptr;
            renderHandler = nullptr;

            CefShutdown();
            SDL_DestroyRenderer(renderer);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
