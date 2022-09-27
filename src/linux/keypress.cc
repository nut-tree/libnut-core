#include "../keypress.h"
#include "../deadbeef_rand.h"
#include "../microsleep.h"

#include <cctype> /* For isupper() */
#include <locale>
#include <codecvt>

#include <X11/extensions/XTest.h>
#include "../xdisplay.h"

#define X_KEY_EVENT(display, key, is_press)                \
	(XTestFakeKeyEvent(display,                        \
			   XKeysymToKeycode(display, key), \
			   is_press, CurrentTime),         \
	 XSync(display, false))

void toggleKeyCode(MMKeyCode code, const bool down, MMKeyFlags flags)
{
	Display *display = XGetMainDisplay();
	const Bool is_press = down ? True : False; /* Just to be safe. */

	if (down)
	{
		/* Parse modifier keys. */
		if (flags & MOD_META)
			X_KEY_EVENT(display, K_META, is_press);
		if (flags & MOD_ALT)
			X_KEY_EVENT(display, K_ALT, is_press);
		if (flags & MOD_CONTROL)
			X_KEY_EVENT(display, K_CONTROL, is_press);
		if (flags & MOD_SHIFT)
			X_KEY_EVENT(display, K_SHIFT, is_press);

		X_KEY_EVENT(display, code, is_press);
	}
	else
	{
		X_KEY_EVENT(display, code, is_press);

		/* Parse modifier keys. */
		if (flags & MOD_META)
			X_KEY_EVENT(display, K_META, is_press);
		if (flags & MOD_ALT)
			X_KEY_EVENT(display, K_ALT, is_press);
		if (flags & MOD_CONTROL)
			X_KEY_EVENT(display, K_CONTROL, is_press);
		if (flags & MOD_SHIFT)
			X_KEY_EVENT(display, K_SHIFT, is_press);
	}
}

void tapKeyCode(MMKeyCode code, MMKeyFlags flags)
{
	toggleKeyCode(code, true, flags);
	toggleKeyCode(code, false, flags);
}

void toggleKey(char c, const bool down, MMKeyFlags flags)
{
	MMKeyCode keyCode = keyCodeForChar(c);

	//Prevent unused variable warning for Mac and Linux.

	if (isupper(c) && !(flags & MOD_SHIFT))
	{
		flags |= MOD_SHIFT; /* Not sure if this is safe for all layouts. */
	}

	toggleKeyCode(keyCode, down, flags);
}

void tapKey(char c, MMKeyFlags flags)
{
	toggleKey(c, true, flags);
	toggleKey(c, false, flags);
}

void typeString(const std::u16string &str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
	std::string utf8String = conv.to_bytes(str);

	xdo_enter_text_window(get_xdo(), 0, utf8String.c_str(), 0);
}
