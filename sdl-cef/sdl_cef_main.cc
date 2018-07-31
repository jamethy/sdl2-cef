
#include <iostream>
#include "SDL.h"
#include "include/cef_browser.h"
#include "include/cef_app.h"

#include "sdl_cef_render_handler.h"
#include "sdl_cef_browser_client.h"
#include "sdl_key_utils.h"

CefBrowserHost::MouseButtonType translateMouseButton(SDL_MouseButtonEvent const &e) {
    CefBrowserHost::MouseButtonType result;
    switch (e.button) {
        case SDL_BUTTON_LEFT:
        case SDL_BUTTON_X1:
            result = MBT_LEFT;
            break;

        case SDL_BUTTON_MIDDLE:
            result = MBT_MIDDLE;
            break;

        case SDL_BUTTON_RIGHT:
        case SDL_BUTTON_X2:
            result = MBT_RIGHT;
            break;
    }
    return result;
}

int main(int argc, char *argv[]) {
    CefMainArgs args(argc, argv);

    int processResult = CefExecuteProcess(args, nullptr, nullptr);
    // checkout CefApp, derive it and set it as second parameter, for more control on
    // command args and resources.
    if (processResult >= 0) {
        return processResult;
    }

    CefSettings settings;

    settings.windowless_rendering_enabled = true;

    bool result = CefInitialize(args, settings, nullptr, nullptr);
    // CefInitialize creates a sub-proccess and executes the same executeable, as calling CefInitialize, if not set different in settings.browser_subprocess_path
    // if you create an extra program just for the childproccess you only have to call CefExecuteProcess(...) in it.
    if (!result) {
        // handle error
        return -1;
    }

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int width = 800;
    int height = 600;

    auto window = SDL_CreateWindow("Render CEF with SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,
                                   height, SDL_WINDOW_RESIZABLE);
    if (window) {
        auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer) {
            SDL_Event e;

            CefRefPtr<SdlCefRenderHandler> renderHandler = new SdlCefRenderHandler(renderer, width, height);

            // create browser-window
            CefRefPtr<CefBrowser> browser;
            CefRefPtr<SdlCefBrowserClient> browserClient;

            {
                CefWindowInfo window_info;
                CefBrowserSettings browserSettings;

                // browserSettings.windowless_frame_rate = 60; // 30 is default

                //window_info.SetAsWindowless(NULL, false); // false means no transparency (site background colour)
                window_info.SetAsWindowless(0); // false means no transparency (site background colour)

                browserClient = new SdlCefBrowserClient(renderHandler);

                browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "http://www.google.com",
                                                            browserSettings, nullptr);

                // inject user-input by calling - non-trivial for non-windows - checkout the cefclient source and the platform specific cpp, like cefclient_osr_widget_gtk.cpp for linux
                // browser->GetHost()->SendKeyEvent(...);
                // browser->GetHost()->SendMouseMoveEvent(...);
                // browser->GetHost()->SendMouseClickEvent(...);
                // browser->GetHost()->SendMouseWheelEvent(...);
            }

            bool shutdown = false;
            bool js_executed = false;
            while (!browserClient->closeAllowed()) {
                // send events to browser
                while (!shutdown && SDL_PollEvent(&e) != 0) {
                    switch (e.type) {
                        case SDL_QUIT:
                            shutdown = true;
                            browser->GetHost()->CloseBrowser(false);
                            break;

                        case SDL_KEYDOWN: {
                            CefKeyEvent event;
                            event.modifiers = getKeyboardModifiers(e.key.keysym.mod);
                            event.windows_key_code = getWindowsKeyCode(e.key.keysym);

                            event.type = KEYEVENT_RAWKEYDOWN;
                            browser->GetHost()->SendKeyEvent(event);

                            event.type = KEYEVENT_CHAR;
                            browser->GetHost()->SendKeyEvent(event);
                        }
                            break;

                        case SDL_KEYUP: {
                            CefKeyEvent event;
                            event.modifiers = getKeyboardModifiers(e.key.keysym.mod);
                            event.windows_key_code = getWindowsKeyCode(e.key.keysym);

                            event.type = KEYEVENT_KEYUP;

                            browser->GetHost()->SendKeyEvent(event);
                        }
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
                    }
                }

                if (!js_executed && browserClient->isLoaded()) {
                    js_executed = true;

                    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
                    frame->ExecuteJavaScript("alert('ExecuteJavaScript works!');", frame->GetURL(), 0);
                }

                // let browser process events
                CefDoMessageLoopWork();

                // render
                SDL_RenderClear(renderer);

                renderHandler->render();

                // Update screen
                SDL_RenderPresent(renderer);
            }

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
