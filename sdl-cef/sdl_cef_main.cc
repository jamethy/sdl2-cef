
#include "sdl_cef_render_handler.h"
#include "sdl_cef_browser_client.h"
#include "sdl_cef_app.h"
#include "sdl_key_utils.h"

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

const int INITIAL_WINDOW_WIDTH = 800;
const int INITIAL_WINDOW_HEIGHT = 600;

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
                    //browser->GetHost()->SetWindowVisibility(false);
                    //browser->GetHost()->WasHidden(true);
                    break;

                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_RESTORED:
                    //browser->GetHost()->SetWindowVisibility(true);
                    //browser->GetHost()->WasHidden(false);
                    break;

                case SDL_WINDOWEVENT_CLOSE:
                    e.type = SDL_QUIT;
                    SDL_PushEvent(&e);
                    break;
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

void makeBackground(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture) {

    int w = 0, h = 0;
    SDL_GetWindowSize(window, &w, &h);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> overSize(10, w / 5);
    std::uniform_int_distribution<int> overWidth(0, w);
    std::uniform_int_distribution<int> overHeight(0, h);
    std::uniform_int_distribution<int> overAngle(0, 360);

    for (int i = 0; i < 50; ++i) {

        SDL_Rect rect;
        rect.w = overSize(generator);
        rect.h = rect.w;
        rect.x = overWidth(generator);
        rect.y = overHeight(generator);
        int angle = overAngle(generator);

        SDL_RenderCopyEx(renderer, texture,
                         nullptr, &rect,
                         angle,
                         nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
    }
}

int main(int argc, char *argv[]) {

    // This block of code is called first because CEF will call this executable
    // to start separate processes. So anything above this point would be called multiple times.
    CefMainArgs args(argc, argv);
    CefRefPtr<SdlCefApp> cefApp = new SdlCefApp();
    int exitCode = CefExecuteProcess(args, cefApp, nullptr);
    if (exitCode >= 0) {
        return exitCode;
    }

    // Initialize Chromium Embedded Framework
    CefSettings settings;
    settings.windowless_rendering_enabled = true;

    if (!CefInitialize(args, settings, cefApp, nullptr)) {
        return -1;
    }

    // Initialize Simple DirectMedia Layer
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    auto window = SDL_CreateWindow("Render CEF with SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   INITIAL_WINDOW_WIDTH,
                                   INITIAL_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

    if (window) {
        auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer) {
            SDL_Event e;

            CefRefPtr<SdlCefRenderHandler> renderHandler = new SdlCefRenderHandler(renderer, INITIAL_WINDOW_WIDTH,
                                                                                   INITIAL_WINDOW_HEIGHT);
            // create browser-window
            CefRefPtr<CefBrowser> browser;
            CefRefPtr<SdlCefBrowserClient> browserClient;

            {
                CefWindowInfo window_info;
                window_info.SetAsWindowless(kNullWindowHandle);
                window_info.windowless_rendering_enabled = true;

                CefBrowserSettings browserSettings;
                browserSettings.background_color = CefColorSetARGB(0, 100, 0, 0);

                browserClient = new SdlCefBrowserClient(renderHandler);
                std::string htmlFile = "file://" + std::string(SDL_GetBasePath()) + "sdl_cef_html.html";

                browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(),
                                                            htmlFile,
                                                            browserSettings, nullptr);
            }

            std::string imgPath = std::string(SDL_GetBasePath()) + "sdl_cef_img.png";
            SDL_Texture *texture = IMG_LoadTexture(renderer, imgPath.c_str());
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

            bool shutdown = false;
            while (!browserClient->closeAllowed()) {

                auto start = steady_clock::now();

                // send events to browser
                while (!shutdown && SDL_PollEvent(&e) != 0) {
                    shutdown = handleEvent(e, browser.get(), renderHandler.get());
                }

                // let browser process events
                CefDoMessageLoopWork();

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                // render
                SDL_RenderClear(renderer);

                makeBackground(window, renderer, texture);

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
