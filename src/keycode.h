#pragma once
#ifndef KEYCODE_H
#define KEYCODE_H

#include "os.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(IS_MACOSX)

#include <Carbon/Carbon.h> /* Really only need <HIToolbox/Events.h> */
#include <ApplicationServices/ApplicationServices.h>
#import <IOKit/hidsystem/ev_keymap.h>

enum _MMKeyCode {
    K_NOT_A_KEY = 9999,

    K_BACKSPACE = kVK_Delete,
    K_DELETE = kVK_ForwardDelete,
    K_RETURN = kVK_Return,
    K_TAB = kVK_Tab,
    K_ESCAPE = kVK_Escape,
    K_UP = kVK_UpArrow,
    K_DOWN = kVK_DownArrow,
    K_RIGHT = kVK_RightArrow,
    K_LEFT = kVK_LeftArrow,
    K_HOME = kVK_Home,
    K_END = kVK_End,
    K_PAGEUP = kVK_PageUp,
    K_PAGEDOWN = kVK_PageDown,

    K_A = kVK_ANSI_A,
    K_B = kVK_ANSI_B,
    K_C = kVK_ANSI_C,
    K_D = kVK_ANSI_D,
    K_E = kVK_ANSI_E,
    K_F = kVK_ANSI_F,
    K_G = kVK_ANSI_G,
    K_H = kVK_ANSI_H,
    K_I = kVK_ANSI_I,
    K_J = kVK_ANSI_J,
    K_K = kVK_ANSI_K,
    K_L = kVK_ANSI_L,
    K_M = kVK_ANSI_M,
    K_N = kVK_ANSI_N,
    K_O = kVK_ANSI_O,
    K_P = kVK_ANSI_P,
    K_Q = kVK_ANSI_Q,
    K_R = kVK_ANSI_R,
    K_S = kVK_ANSI_S,
    K_T = kVK_ANSI_T,
    K_U = kVK_ANSI_U,
    K_V = kVK_ANSI_V,
    K_W = kVK_ANSI_W,
    K_X = kVK_ANSI_X,
    K_Y = kVK_ANSI_Y,
    K_Z = kVK_ANSI_Z,

    K_COMMA = kVK_ANSI_Comma,
    K_PERIOD = kVK_ANSI_Period,
    K_SLASH = kVK_ANSI_Slash,

    K_SEMICOLON = kVK_ANSI_Semicolon,
    K_QUOTE = kVK_ANSI_Quote,
    K_LEFTBRACKET = kVK_ANSI_LeftBracket,
    K_RIGHTBRACKET = kVK_ANSI_RightBracket,
    K_BACKSLASH = kVK_ANSI_Backslash,

    K_MINUS = kVK_ANSI_Minus,
    K_EQUAL = kVK_ANSI_Equal,

    K_GRAVE = kVK_ANSI_Grave,

    K_F1 = kVK_F1,
    K_F2 = kVK_F2,
    K_F3 = kVK_F3,
    K_F4 = kVK_F4,
    K_F5 = kVK_F5,
    K_F6 = kVK_F6,
    K_F7 = kVK_F7,
    K_F8 = kVK_F8,
    K_F9 = kVK_F9,
    K_F10 = kVK_F10,
    K_F11 = kVK_F11,
    K_F12 = kVK_F12,
    K_F13 = kVK_F13,
    K_F14 = kVK_F14,
    K_F15 = kVK_F15,
    K_F16 = kVK_F16,
    K_F17 = kVK_F17,
    K_F18 = kVK_F18,
    K_F19 = kVK_F19,
    K_F20 = kVK_F20,
    K_F21 = K_NOT_A_KEY,
    K_F22 = K_NOT_A_KEY,
    K_F23 = K_NOT_A_KEY,
    K_F24 = K_NOT_A_KEY,

    K_META = kVK_Command,
    K_RIGHTMETA = kVK_RightCommand,

    K_CMD = kVK_Command,
    K_RIGHTCMD = kVK_RightCommand,

    K_WIN = K_NOT_A_KEY,
    K_RIGHTWIN = K_NOT_A_KEY,

    K_FUNCTION = kVK_Function,
    K_PAUSE = K_NOT_A_KEY,

    K_ALT = kVK_Option,
    K_RIGHTALT = kVK_RightOption,

    K_CONTROL = kVK_Control,
    K_RIGHTCONTROL = kVK_RightControl,

    K_SHIFT = kVK_Shift,
    K_RIGHTSHIFT = kVK_RightShift,

    K_CAPSLOCK = kVK_CapsLock,
    K_NUMLOCK = K_NOT_A_KEY,
    K_SCROLL_LOCK = K_NOT_A_KEY,

    K_SPACE = kVK_Space,
    K_INSERT = K_NOT_A_KEY,
    K_PRINTSCREEN = K_NOT_A_KEY,
    K_MENU = K_NOT_A_KEY,

    K_ADD = kVK_ANSI_KeypadPlus,
    K_SUBTRACT = kVK_ANSI_KeypadMinus,
    K_DIVIDE = kVK_ANSI_KeypadDivide,
    K_MULTIPLY = kVK_ANSI_KeypadMultiply,
    K_ENTER = kVK_ANSI_KeypadEnter,
    K_CLEAR = kVK_ANSI_KeypadClear,

    K_0 = kVK_ANSI_0,
    K_1 = kVK_ANSI_1,
    K_2 = kVK_ANSI_2,
    K_3 = kVK_ANSI_3,
    K_4 = kVK_ANSI_4,
    K_5 = kVK_ANSI_5,
    K_6 = kVK_ANSI_6,
    K_7 = kVK_ANSI_7,
    K_8 = kVK_ANSI_8,
    K_9 = kVK_ANSI_9,

    K_NUMPAD_0 = kVK_ANSI_Keypad0,
    K_NUMPAD_1 = kVK_ANSI_Keypad1,
    K_NUMPAD_2 = kVK_ANSI_Keypad2,
    K_NUMPAD_3 = kVK_ANSI_Keypad3,
    K_NUMPAD_4 = kVK_ANSI_Keypad4,
    K_NUMPAD_5 = kVK_ANSI_Keypad5,
    K_NUMPAD_6 = kVK_ANSI_Keypad6,
    K_NUMPAD_7 = kVK_ANSI_Keypad7,
    K_NUMPAD_8 = kVK_ANSI_Keypad8,
    K_NUMPAD_9 = kVK_ANSI_Keypad9,
    K_NUMPAD_DECIMAL = kVK_ANSI_KeypadDecimal,
    K_NUMPAD_EQUAL = kVK_ANSI_KeypadEquals,

    K_AUDIO_VOLUME_MUTE = 1007,
    K_AUDIO_VOLUME_DOWN = 1001,
    K_AUDIO_VOLUME_UP = 1000,
    K_AUDIO_PLAY = 1016,
    K_AUDIO_STOP = K_NOT_A_KEY,
    K_AUDIO_PAUSE = 1016,
    K_AUDIO_PREV = 1018,
    K_AUDIO_NEXT = 1017,
    K_AUDIO_REWIND = K_NOT_A_KEY,
    K_AUDIO_FORWARD = K_NOT_A_KEY,
    K_AUDIO_REPEAT = K_NOT_A_KEY,
    K_AUDIO_RANDOM = K_NOT_A_KEY,

    K_LIGHTS_MON_UP = 1002,
    K_LIGHTS_MON_DOWN = 1003,
    K_LIGHTS_KBD_TOGGLE = 1023,
    K_LIGHTS_KBD_UP = 1021,
    K_LIGHTS_KBD_DOWN = 1022
};

typedef CGKeyCode MMKeyCode;

#elif defined(USE_X11)

#include <X11/Xutil.h>
#include <X11/XF86keysym.h>

enum _MMKeyCode {
    K_NOT_A_KEY = 9999,

    K_BACKSPACE = XK_BackSpace,
    K_DELETE = XK_Delete,
    K_RETURN = XK_Return,
    K_TAB = XK_Tab,
    K_ESCAPE = XK_Escape,

    K_UP = XK_Up,
    K_DOWN = XK_Down,
    K_RIGHT = XK_Right,
    K_LEFT = XK_Left,

    K_HOME = XK_Home,
    K_END = XK_End,
    K_PAGEUP = XK_Page_Up,
    K_PAGEDOWN = XK_Page_Down,

    K_F1 = XK_F1,
    K_F2 = XK_F2,
    K_F3 = XK_F3,
    K_F4 = XK_F4,
    K_F5 = XK_F5,
    K_F6 = XK_F6,
    K_F7 = XK_F7,
    K_F8 = XK_F8,
    K_F9 = XK_F9,
    K_F10 = XK_F10,
    K_F11 = XK_F11,
    K_F12 = XK_F12,
    K_F13 = XK_F13,
    K_F14 = XK_F14,
    K_F15 = XK_F15,
    K_F16 = XK_F16,
    K_F17 = XK_F17,
    K_F18 = XK_F18,
    K_F19 = XK_F19,
    K_F20 = XK_F20,
    K_F21 = XK_F21,
    K_F22 = XK_F22,
    K_F23 = XK_F23,
    K_F24 = XK_F24,

    K_A = XK_A,
    K_B = XK_B,
    K_C = XK_C,
    K_D = XK_D,
    K_E = XK_E,
    K_F = XK_F,
    K_G = XK_G,
    K_H = XK_H,
    K_I = XK_I,
    K_J = XK_J,
    K_K = XK_K,
    K_L = XK_L,
    K_M = XK_M,
    K_N = XK_N,
    K_O = XK_O,
    K_P = XK_P,
    K_Q = XK_Q,
    K_R = XK_R,
    K_S = XK_S,
    K_T = XK_T,
    K_U = XK_U,
    K_V = XK_V,
    K_W = XK_W,
    K_X = XK_X,
    K_Y = XK_Y,
    K_Z = XK_Z,

    K_COMMA = XK_comma,
    K_PERIOD = XK_period,
    K_SLASH = XK_slash,

    K_SEMICOLON = XK_semicolon,
    K_QUOTE = XK_apostrophe,
    K_LEFTBRACKET = XK_bracketleft,
    K_RIGHTBRACKET = XK_bracketright,
    K_BACKSLASH = XK_backslash,

    K_MINUS = XK_minus,
    K_EQUAL = XK_equal,

    K_GRAVE = XK_grave,

    K_META = XK_Super_L,
    K_RIGHTMETA = XK_Super_R,

    K_CMD = K_NOT_A_KEY,
    K_RIGHTCMD = K_NOT_A_KEY,

    K_WIN = XK_Super_L,
    K_RIGHTWIN = XK_Super_R,

    K_ALT = XK_Alt_L,
    K_RIGHTALT = XK_Alt_R,

    K_FUNCTION = K_NOT_A_KEY,
    K_PAUSE = XK_Pause,

    K_CONTROL = XK_Control_L,
    K_RIGHTCONTROL = XK_Control_R,

    K_SHIFT = XK_Shift_L,
    K_RIGHTSHIFT = XK_Shift_R,

    K_CAPSLOCK = XK_Caps_Lock,
    K_NUMLOCK = XK_Num_Lock,
    K_SCROLL_LOCK = XK_Scroll_Lock,

    K_SPACE = XK_space,
    K_INSERT = XK_Insert,
    K_PRINTSCREEN = XK_Print,
    K_MENU = K_NOT_A_KEY,

    K_ADD = XK_KP_Add,
    K_SUBTRACT = XK_KP_Subtract,
    K_DIVIDE = XK_KP_Divide,
    K_MULTIPLY = XK_KP_Multiply,
    K_ENTER = XK_KP_Enter,
    K_CLEAR = XK_Clear,

    K_0 = XK_0,
    K_1 = XK_1,
    K_2 = XK_2,
    K_3 = XK_3,
    K_4 = XK_4,
    K_5 = XK_5,
    K_6 = XK_6,
    K_7 = XK_7,
    K_8 = XK_8,
    K_9 = XK_9,

    K_NUMPAD_0 = XK_KP_0,
    K_NUMPAD_1 = XK_KP_1,
    K_NUMPAD_2 = XK_KP_2,
    K_NUMPAD_3 = XK_KP_3,
    K_NUMPAD_4 = XK_KP_4,
    K_NUMPAD_5 = XK_KP_5,
    K_NUMPAD_6 = XK_KP_6,
    K_NUMPAD_7 = XK_KP_7,
    K_NUMPAD_8 = XK_KP_8,
    K_NUMPAD_9 = XK_KP_9,
    K_NUMPAD_DECIMAL = XK_KP_Decimal,
    K_NUMPAD_EQUAL = XK_KP_Equal,

    K_AUDIO_VOLUME_DOWN = XF86XK_AudioLowerVolume,
    K_AUDIO_VOLUME_UP = XF86XK_AudioRaiseVolume,
    K_AUDIO_VOLUME_MUTE = XF86XK_AudioMute,
    K_AUDIO_PLAY = XF86XK_AudioPlay,
    K_AUDIO_STOP = XF86XK_AudioStop,
    K_AUDIO_PAUSE = XF86XK_AudioPause,
    K_AUDIO_PREV = XF86XK_AudioPrev,
    K_AUDIO_NEXT = XF86XK_AudioNext,
    K_AUDIO_REWIND = XF86XK_AudioRewind,
    K_AUDIO_FORWARD = XF86XK_AudioForward,
    K_AUDIO_REPEAT = XF86XK_AudioRepeat,
    K_AUDIO_RANDOM = XF86XK_AudioRandomPlay,

    K_LIGHTS_MON_DOWN = XF86XK_MonBrightnessDown,
    K_LIGHTS_MON_UP = XF86XK_MonBrightnessUp,
    K_LIGHTS_KBD_TOGGLE = XF86XK_KbdLightOnOff,
    K_LIGHTS_KBD_DOWN = XF86XK_KbdBrightnessDown,
    K_LIGHTS_KBD_UP = XF86XK_KbdBrightnessUp
};

typedef KeySym MMKeyCode;

#elif defined(IS_WINDOWS)

#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

enum _MMKeyCode {
  K_NOT_A_KEY = 9999,

  K_BACKSPACE = VK_BACK,
  K_DELETE = VK_DELETE,
  K_RETURN = VK_RETURN,
  K_TAB = VK_TAB,
  K_ESCAPE = VK_ESCAPE,

  K_UP = VK_UP,
  K_DOWN = VK_DOWN,
  K_RIGHT = VK_RIGHT,
  K_LEFT = VK_LEFT,

  K_HOME = VK_HOME,
  K_END = VK_END,
  K_PAGEUP = VK_PRIOR,
  K_PAGEDOWN = VK_NEXT,

  K_F1 = VK_F1,
  K_F2 = VK_F2,
  K_F3 = VK_F3,
  K_F4 = VK_F4,
  K_F5 = VK_F5,
  K_F6 = VK_F6,
  K_F7 = VK_F7,
  K_F8 = VK_F8,
  K_F9 = VK_F9,
  K_F10 = VK_F10,
  K_F11 = VK_F11,
  K_F12 = VK_F12,
  K_F13 = VK_F13,
  K_F14 = VK_F14,
  K_F15 = VK_F15,
  K_F16 = VK_F16,
  K_F17 = VK_F17,
  K_F18 = VK_F18,
  K_F19 = VK_F19,
  K_F20 = VK_F20,
  K_F21 = VK_F21,
  K_F22 = VK_F22,
  K_F23 = VK_F23,
  K_F24 = VK_F24,

  K_A = VK_A,
  K_B = VK_B,
  K_C = VK_C,
  K_D = VK_D,
  K_E = VK_E,
  K_F = VK_F,
  K_G = VK_G,
  K_H = VK_H,
  K_I = VK_I,
  K_J = VK_J,
  K_K = VK_K,
  K_L = VK_L,
  K_M = VK_M,
  K_N = VK_N,
  K_O = VK_O,
  K_P = VK_P,
  K_Q = VK_Q,
  K_R = VK_R,
  K_S = VK_S,
  K_T = VK_T,
  K_U = VK_U,
  K_V = VK_V,
  K_W = VK_W,
  K_X = VK_X,
  K_Y = VK_Y,
  K_Z = VK_Z,

  K_COMMA = VK_OEM_COMMA,
  K_PERIOD = VK_OEM_PERIOD,
  K_SLASH = VK_OEM_2,

  K_SEMICOLON = VK_OEM_1,
  K_QUOTE = VK_OEM_7,
  K_LEFTBRACKET = VK_OEM_4,
  K_RIGHTBRACKET = VK_OEM_6,
  K_BACKSLASH = VK_OEM_5,

  K_MINUS = VK_OEM_MINUS,
  K_EQUAL = VK_OEM_PLUS,

  K_GRAVE = VK_OEM_3,

  K_META = VK_LWIN,
  K_RIGHTMETA = VK_RWIN,

  K_CMD = K_NOT_A_KEY,
  K_RIGHTCMD = K_NOT_A_KEY,

  K_WIN = VK_LWIN,
  K_RIGHTWIN = VK_RWIN,

  K_FUNCTION = K_NOT_A_KEY,
  K_PAUSE = VK_PAUSE,

  K_ALT = VK_LMENU,
  K_RIGHTALT = VK_RMENU,

  K_CONTROL = VK_LCONTROL,
  K_RIGHTCONTROL = VK_RCONTROL,

  K_SHIFT = VK_LSHIFT,
  K_RIGHTSHIFT = VK_RSHIFT,

  K_CAPSLOCK = VK_CAPITAL,
  K_NUMLOCK = VK_NUMLOCK,
  K_SCROLL_LOCK = VK_SCROLL,

  K_SPACE = VK_SPACE,

  K_PRINTSCREEN = VK_SNAPSHOT,
  K_INSERT = VK_INSERT,
  K_MENU = VK_APPS,

  K_ADD = VK_ADD,
  K_SUBTRACT = VK_SUBTRACT,
  K_DIVIDE = VK_DIVIDE,
  K_MULTIPLY = VK_MULTIPLY,
  K_ENTER = VK_RETURN,
  K_CLEAR = VK_CLEAR,

  K_0 = VK_0,
  K_1 = VK_1,
  K_2 = VK_2,
  K_3 = VK_3,
  K_4 = VK_4,
  K_5 = VK_5,
  K_6 = VK_6,
  K_7 = VK_7,
  K_8 = VK_8,
  K_9 = VK_9,

  K_NUMPAD_0 = VK_NUMPAD0,
  K_NUMPAD_1 = VK_NUMPAD1,
  K_NUMPAD_2 = VK_NUMPAD2,
  K_NUMPAD_3 = VK_NUMPAD3,
  K_NUMPAD_4 = VK_NUMPAD4,
  K_NUMPAD_5 = VK_NUMPAD5,
  K_NUMPAD_6 = VK_NUMPAD6,
  K_NUMPAD_7 = VK_NUMPAD7,
  K_NUMPAD_8 = VK_NUMPAD8,
  K_NUMPAD_9 = VK_NUMPAD9,
  K_NUMPAD_DECIMAL = VK_DECIMAL,
  K_NUMPAD_EQUAL = K_NOT_A_KEY,

  K_AUDIO_VOLUME_MUTE = VK_VOLUME_MUTE,
  K_AUDIO_VOLUME_DOWN = VK_VOLUME_DOWN,
  K_AUDIO_VOLUME_UP = VK_VOLUME_UP,
  K_AUDIO_PLAY = VK_MEDIA_PLAY_PAUSE,
  K_AUDIO_STOP = VK_MEDIA_STOP,
  K_AUDIO_PAUSE = VK_MEDIA_PLAY_PAUSE,
  K_AUDIO_PREV = VK_MEDIA_PREV_TRACK,
  K_AUDIO_NEXT = VK_MEDIA_NEXT_TRACK,
  K_AUDIO_REWIND = K_NOT_A_KEY,
  K_AUDIO_FORWARD = K_NOT_A_KEY,
  K_AUDIO_REPEAT = K_NOT_A_KEY,
  K_AUDIO_RANDOM = K_NOT_A_KEY,

  K_LIGHTS_MON_UP = K_NOT_A_KEY,
  K_LIGHTS_MON_DOWN = K_NOT_A_KEY,
  K_LIGHTS_KBD_TOGGLE = K_NOT_A_KEY,
  K_LIGHTS_KBD_UP = K_NOT_A_KEY,
  K_LIGHTS_KBD_DOWN = K_NOT_A_KEY,
};

typedef int MMKeyCode;

#endif

/* Returns the keyCode corresponding to the current keyboard layout for the
 * given ASCII character. */
MMKeyCode keyCodeForChar(const char c);

#ifdef __cplusplus
}
#endif

#endif /* KEYCODE_H */

