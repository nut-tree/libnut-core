#pragma once
#ifndef KEYPRESS_H
#define KEYPRESS_H

#include "os.h"
#include "keycode.h"
#if defined(USE_X11)
#include "xdisplay.h"
#endif

#include <string>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" 
{
#endif
#if defined(IS_MACOSX)

	typedef enum  {
		MOD_NONE = 0,
		MOD_META = kCGEventFlagMaskCommand,
		MOD_ALT = kCGEventFlagMaskAlternate,
		MOD_CONTROL = kCGEventFlagMaskControl,
		MOD_SHIFT = kCGEventFlagMaskShift
	} MMKeyFlags;

	extern MMKeyFlags flagBuffer;

#elif defined(USE_X11)

	enum _MMKeyFlags {
		MOD_NONE = 0,
		MOD_META = Mod4Mask,
		MOD_ALT = Mod1Mask,
		MOD_CONTROL = ControlMask,
		MOD_SHIFT = ShiftMask
	};

	typedef unsigned int MMKeyFlags;

#elif defined(IS_WINDOWS)

	enum _MMKeyFlags {
		MOD_NONE = 0,
		/* These are already defined by the Win32 API */
		/* MOD_ALT = 0,
		MOD_CONTROL = 0,
		MOD_SHIFT = 0, */
		MOD_META = MOD_WIN
	};

	typedef unsigned int MMKeyFlags;

#endif

#if defined(IS_WINDOWS)
/* Send win32 key event for given key. */
void win32KeyEvent(int key, MMKeyFlags flags);
#endif

/* Toggles the given key down or up. */
void toggleKeyCode(MMKeyCode code, const bool down, MMKeyFlags flags);

/* Toggles the key down and then up. */
void tapKeyCode(MMKeyCode code, MMKeyFlags flags);

/* Toggles the key corresponding to the given UTF character up or down. */
void toggleKey(char c, const bool down, MMKeyFlags flags);
void tapKey(char c, MMKeyFlags flags);

/* Sends a UTF-8 string without modifiers. */
void typeString(const std::u16string &str);

#ifdef __cplusplus
}
#endif

#endif /* KEYPRESS_H */
