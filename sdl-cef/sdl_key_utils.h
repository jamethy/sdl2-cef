#ifndef _KEYBOARD_UTILS_H_
#define _KEYBOARD_UTILS_H_

#include "include/internal/cef_types.h"
#include "include/cef_browser.h"

#include "SDL.h"

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


void handleKeyEvent(SDL_Event event, CefBrowser *browser) {
    /** Modifiers **/
    bool shift = (event.key.keysym.mod & KMOD_LSHIFT) || (event.key.keysym.mod & KMOD_RSHIFT);
    bool ctrl = (event.key.keysym.mod & KMOD_LCTRL) || (event.key.keysym.mod & KMOD_RCTRL);
    bool alt = (event.key.keysym.mod & KMOD_LALT) || (event.key.keysym.mod & KMOD_RALT);
    bool num_lock = !(event.key.keysym.mod & KMOD_NUM);
    bool caps_lock = static_cast<bool>(event.key.keysym.mod & KMOD_CAPS);
    bool uppercase = caps_lock == !shift;

    int modifiers = 0;
    if (shift)
        modifiers += EVENTFLAG_SHIFT_DOWN;
    if (ctrl)
        modifiers += EVENTFLAG_CONTROL_DOWN;
    if (alt)
        modifiers += EVENTFLAG_ALT_DOWN;
    if (num_lock)
        modifiers += EVENTFLAG_NUM_LOCK_ON;
    if (caps_lock)
        modifiers += EVENTFLAG_CAPS_LOCK_ON;

    /** Output codes **/
    int key_code = 0;
    int char_code = -1;

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {

        SDL_Keycode code = event.key.keysym.sym;
        if (code >= SDLK_a && code <= SDLK_z) {
            int diff = code - SDLK_a;
            key_code = 65 + diff;
            char_code = (uppercase ? 'A' : 'a') + diff;
        } else if (code >= SDLK_0 && code <= SDLK_9) {
            int diff = code - SDLK_0;
            key_code = 48 + diff;
            char_code = '0' + diff;

            if (uppercase) {
                switch (code) {
                    case SDLK_1:
                        char_code = '!';
                        break;
                    case SDLK_2:
                        char_code = '@';
                        break;
                    case SDLK_3:
                        char_code = '#';
                        break;
                    case SDLK_4:
                        char_code = '$';
                        break;
                    case SDLK_5:
                        char_code = '%';
                        break;
                    case SDLK_6:
                        char_code = '^';
                        break;
                    case SDLK_7:
                        char_code = '&';
                        break;
                    case SDLK_8:
                        char_code = '*';
                        break;
                    case SDLK_9:
                        char_code = '(';
                        break;
                    case SDLK_0:
                        char_code = ')';
                        break;
                    default:
                        break;
                }
            }

            /** Function Keys **/
        } else if (code >= SDLK_F1 && code <= SDLK_F12) {
            key_code = 112 + (code - 1073741882);
        } else {
            /** Special Keys **/
            switch (code) {
                case SDLK_SPACE:
                    key_code = 32;
                    char_code = ' ';
                    break;
                case SDLK_BACKSPACE:
                    key_code = 8;
                    break;
                case SDLK_TAB:
                    key_code = 9;
                    break;
                case SDLK_RETURN:
                    key_code = 13;
                    char_code = 13;
                    break;
                case SDLK_PAUSE:
                    key_code = 19;
                    break;
                case _SDLK_SCROLL_LOCK:
                    key_code = JSK_SCROLL_LOCK;
                    break;
                case SDLK_ESCAPE:
                    key_code = 27;
                    break;
                case SDLK_LEFT:
                    key_code = 37;
                    break;
                case SDLK_UP:
                    key_code = 38;
                    break;
                case SDLK_RIGHT:
                    key_code = 39;
                    break;
                case SDLK_DOWN:
                    key_code = 40;
                    break;
                case _SDLK_HOME:
                    key_code = JSK_HOME;
                    break;
                case _SDLK_END:
                    key_code = JSK_END;
                    break;
                case _SDLK_PAGEUP:
                    key_code = JSK_PAGE_UP;
                    break;
                case _SDLK_PAGEDOWN:
                    key_code = JSK_PAGE_DOWN;
                    break;
                case _SDLK_INSERT:
                    key_code = JSK_INSERT;
                    break;
                case SDLK_DELETE:
                    key_code = JSK_DELETE;
                    char_code = JSC_DELETE;
                    break;
                case _SDLK_NUM_DIVIDE:
                    key_code = JSK_DIVIDE;
                    char_code = JSC_NUM_DIVIDE;
                    break;
                case _SDLK_NUM_MULTIPLY:
                    key_code = JSK_MULTIPLY;
                    char_code = JSC_NUM_MULTIPLY;
                    break;
                case _SDLK_NUM_SUBTRACT:
                    key_code = JSK_SUBTRACT;
                    char_code = JSC_NUM_SUBTRACT;
                    break;
                case _SDLK_NUM_ADD:
                    key_code = JSK_ADD;
                    char_code = JSC_NUM_ADD;
                    break;
                case _SDLK_NUM_DECIMAL:
                    if (num_lock) {
                        key_code = JSK_DECIMAL_POINT; // keyboard layout dependent!
                        char_code = JSK_DELETE;
                    } else {
                        key_code = JSK_DELETE;
                    }
                    break;
                case _SDLK_KP0:
                    key_code = JSK_INSERT;
                    break;
                case _SDLK_KP1:
                    key_code = JSK_END;
                    break;
                case _SDLK_KP2:
                    key_code = JSK_DOWN_ARROW;
                    break;
                case _SDLK_KP3:
                    key_code = JSK_PAGE_DOWN;
                    break;
                case _SDLK_KP4:
                    key_code = JSK_LEFT_ARROW;
                    break;
                case _SDLK_KP5:
                    key_code = JSK_NUM5_SECOND;
                    break;
                case _SDLK_KP6:
                    key_code = JSK_RIGHT_ARROW;
                    break;
                case _SDLK_KP7:
                    key_code = JSK_HOME;
                    break;
                case _SDLK_KP8:
                    key_code = JSK_UP_ARROW;
                    break;
                case _SDLK_KP9:
                    key_code = JSK_PAGE_UP;
                    break;
                case SDLK_CAPSLOCK:
                    key_code = 20;
                    break;
                case _SDLK_NUM:
                    key_code = JSK_NUM_LOCK;
                    break;
                case SDLK_LCTRL:
                case SDLK_RCTRL:
                    key_code = 17;
                    break;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    key_code = 16;
                    break;
                case SDLK_LALT:
                case SDLK_RALT:
                    key_code = 18;
                    break;
                case SDLK_LGUI:
                    key_code = 91;
                    break;
                case SDLK_RGUI:
                    key_code = 92;
                    break;
                case _SDLK_SELECT:
                    key_code = JSK_SELECT;
                    break;
                case SDLK_SEMICOLON:
                    key_code = 186;
                    char_code = shift ? ':' : ';';
                    break;
                case SDLK_QUOTE:
                    key_code = 222;
                    char_code = shift ? '"' : '\'';
                    break;
                case SDLK_EQUALS:
                    key_code = 187;
                    char_code = shift ? '+' : '=';
                    break;
                case SDLK_COMMA:
                    key_code = 188;
                    char_code = shift ? '<' : ',';
                    break;
                case SDLK_MINUS:
                    key_code = 189;
                    char_code = shift ? '_' : '-';
                    break;
                case SDLK_PERIOD:
                    key_code = 190;
                    char_code = shift ? '>' : '.';
                    break;
                case SDLK_SLASH:
                    key_code = 191;
                    char_code = shift ? '?' : '/';
                    break;
                case SDLK_BACKQUOTE:
                    key_code = 192;
                    char_code = shift ? '~' : '`';
                    break;
                case SDLK_LEFTBRACKET:
                    key_code = 219;
                    char_code = shift ? '{' : '[';
                    break;
                case SDLK_BACKSLASH:
                    key_code = 220;
                    char_code = shift ? '|' : '\\';
                    break;
                case SDLK_RIGHTBRACKET:
                    key_code = 221;
                    char_code = shift ? '}' : ']';
                    break;
            }
        }
    }

    /** Still not mapped? **/
    if (key_code == 0) {
        key_code = event.key.keysym.sym;
    }

    /** Fire key events to CEF **/
    if (event.key.state == SDL_PRESSED || event.type == SDL_TEXTINPUT) {
        // onkeydown
        CefKeyEvent key_event_key_down;
        key_event_key_down.type = KEYEVENT_KEYDOWN;
        key_event_key_down.modifiers = static_cast<uint32>(modifiers);
        key_event_key_down.windows_key_code = key_code;
        key_event_key_down.character = static_cast<char16>(char_code);
        key_event_key_down.unmodified_character = static_cast<char16>(char_code);
        browser->GetHost()->SendKeyEvent(key_event_key_down);
        // Fire a second key event for characters only
        if (char_code >= 0) {
            // onkeypress
            CefKeyEvent key_event_char;
            key_event_char.type = KEYEVENT_CHAR;
            key_event_char.modifiers = static_cast<uint32>(modifiers);
            key_event_char.character = static_cast<char16>(char_code);
            key_event_char.unmodified_character = static_cast<char16>(char_code);
            browser->GetHost()->SendKeyEvent(key_event_char);
        }
    } else if (event.key.state == SDL_RELEASED) {
        // onkeyup
        CefKeyEvent key_event_key_up;
        key_event_key_up.type = KEYEVENT_KEYUP;
        key_event_key_up.modifiers = static_cast<uint32>(modifiers);
        key_event_key_up.windows_key_code = key_code;
        key_event_key_up.character = static_cast<char16>(char_code);
        key_event_key_up.unmodified_character = static_cast<char16>(char_code);
        browser->GetHost()->SendKeyEvent(key_event_key_up);
    }
}

#endif // _KEYBOARD_UTILS_H_
