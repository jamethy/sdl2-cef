
#include "sdl_cef_render_handler.h"
#include "sdl_cef_browser_client.h"
#include "sdl_cef_app.h"
#include "sdl_key_utils.h"
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

CefBrowserHost::MouseButtonType translateMouseButton(SDL_MouseButtonEvent const &e) {
    switch (e.button) {
        case SDL_BUTTON_MIDDLE:
            return MBT_MIDDLE;
        case SDL_BUTTON_RIGHT:
        case SDL_BUTTON_X2:
            return MBT_RIGHT;
        case SDL_BUTTON_LEFT:
        case SDL_BUTTON_X1:
        default:
            return MBT_LEFT;
    }
}

const int INITIAL_WINDOW_WIDTH = 1000;
const int INITIAL_WINDOW_HEIGHT = 2000;

bool handleEvent(SDL_Event &e, CefBrowser *browser, SdlCefRenderHandler *renderHandler) {

    switch (e.type) {
        case SDL_QUIT:
            browser->GetHost()->CloseBrowser(false);
            return true;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            handleKeyEvent(e, browser);
            break;
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    renderHandler->resize(e.window.data1, e.window.data2);
                    browser->GetHost()->WasResized();
                    break;

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    browser->GetHost()->SetFocus(true);
                    break;

                case SDL_WINDOWEVENT_FOCUS_LOST:
                    browser->GetHost()->SetFocus(false);
                    break;

                case SDL_WINDOWEVENT_HIDDEN:
                case SDL_WINDOWEVENT_MINIMIZED:
                    browser->GetHost()->WasHidden(true);
                    break;

                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_RESTORED:
                    browser->GetHost()->WasHidden(false);
                    break;

                case SDL_WINDOWEVENT_CLOSE:
                    e.type = SDL_QUIT;
                    SDL_PushEvent(&e);
                    break;
                default:break;
            }
            break;

        case SDL_MOUSEMOTION: {
            CefMouseEvent event;
            event.x = e.motion.x;
            event.y = e.motion.y;

            browser->GetHost()->SendMouseMoveEvent(event, false);
        }
            break;

        case SDL_MOUSEBUTTONUP: {
            CefMouseEvent event;
            event.x = e.button.x;
            event.y = e.button.y;

            browser->GetHost()->SendMouseClickEvent(event, translateMouseButton(e.button), true, 1);
        }
            break;

        case SDL_MOUSEBUTTONDOWN: {
            CefMouseEvent event;
            event.x = e.button.x;
            event.y = e.button.y;

            browser->GetHost()->SendMouseClickEvent(event, translateMouseButton(e.button), false, 1);
        }
            break;

        case SDL_MOUSEWHEEL: {
            int delta_x = e.wheel.x;
            int delta_y = e.wheel.y;

            if (SDL_MOUSEWHEEL_FLIPPED == e.wheel.direction) {
                delta_y *= -1;
            } else {
                delta_x *= -1;
            }

            CefMouseEvent event;
            browser->GetHost()->SendMouseWheelEvent(event, delta_x, delta_y);
        }
            break;
        default:
            break;
    }

    return false;
}

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

            CefRefPtr<SdlCefBrowserClient> browserClient = new SdlCefBrowserClient(renderHandler);

            CefWindowInfo window_info;
            window_info.SetAsWindowless(kNullWindowHandle);
            window_info.windowless_rendering_enabled = true;

            CefBrowserSettings browserSettings;
            browserSettings.background_color = 0;

            std::string htmlFile = "file://" + std::string(SDL_GetBasePath()) + "sdl_cef_html.html";

            CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient,
                                                                              htmlFile,
                                                                              browserSettings, nullptr);

            std::string imgPath = std::string(SDL_GetBasePath()) + "sdl_cef_img.png";
            SDL_Texture *texture = IMG_LoadTexture(renderer, imgPath.c_str());
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

            bool shutdown = false;
            while (!browserClient->closeAllowed()) {

                auto start = steady_clock::now();

                // send events to browser
                while (!shutdown && SDL_PollEvent(&e) != 0) {
                    shutdown = handleEvent(e, browser.get(), renderHandler.get());

                    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        renderHandler->resize(e.window.data1, e.window.data2);
                        background.resize(e.window.data1, e.window.data2);
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
