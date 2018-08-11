//
// Created by james on 7/29/18.
//

#ifndef CEF_SDL_CEF_EVENTS_H
#define CEF_SDL_CEF_EVENTS_H

#include "SDL.h"
#include "include/cef_browser.h"

/**
 * Sends the SDL event right to the browser
 *
 * @param e SDL Event to translate
 * @param browser Browser to send event to
 */
void handleEvent(SDL_Event &e, CefBrowser *browser);

#endif // CEF_SDL_CEF_EVENTS_H
