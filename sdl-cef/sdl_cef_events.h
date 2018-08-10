//
// Created by james on 7/29/18.
//

#ifndef CEF_SDL_CEF_EVENTS_H
#define CEF_SDL_CEF_EVENTS_H

#include "sdl_cef_render_handler.h"
#include "SDL.h"
#include "include/cef_browser.h"
#include "include/internal/cef_types.h"

#include <iostream>

#define JSK_NUM5_SECOND    12
#define JSK_PAGE_UP        33
#define JSK_PAGE_DOWN      34
#define JSK_END            35
#define JSK_HOME           36
#define JSK_LEFT_ARROW     37
#define JSK_UP_ARROW       38
#define JSK_RIGHT_ARROW    39
#define JSK_DOWN_ARROW     40
#define JSK_INSERT         45
#define JSK_DELETE         46
#define JSK_SELECT         93
#define JSK_MULTIPLY      106
#define JSK_ADD           107
#define JSK_SUBTRACT      109
#define JSK_DECIMAL_POINT 110
#define JSK_DIVIDE        111
#define JSK_NUM_LOCK      144
#define JSK_SCROLL_LOCK   145
#define JSC_NUM_MULTIPLY   42
#define JSC_NUM_ADD        43
#define JSC_NUM_SUBTRACT   45
#define JSC_NUM_DIVIDE     47
#define JSC_DELETE        127

#define _SDLK_KP0         1073741912
#define _SDLK_KP1         1073741913
#define _SDLK_KP2         1073741914
#define _SDLK_KP3         1073741915
#define _SDLK_KP4         1073741916
#define _SDLK_KP5         1073741917
#define _SDLK_KP6         1073741918
#define _SDLK_KP7         1073741919
#define _SDLK_KP8         1073741920
#define _SDLK_KP9         1073741921
#define _SDLK_SCROLL_LOCK 1073741895
#define _SDLK_INSERT      1073741897
#define _SDLK_HOME        1073741898
#define _SDLK_PAGEUP      1073741899
#define _SDLK_END         1073741901
#define _SDLK_PAGEDOWN    1073741902
#define _SDLK_NUM         1073741907
#define _SDLK_NUM_DIVIDE  1073741908
#define _SDLK_NUM_MULTIPLY 1073741909
#define _SDLK_NUM_SUBTRACT 1073741910
#define _SDLK_NUM_ADD     1073741911
#define _SDLK_NUM_DECIMAL 1073741923
#define _SDLK_SELECT      1073741925

bool handleEvent(SDL_Event &e, CefBrowser *browser);

#endif // CEF_SDL_CEF_EVENTS_H
