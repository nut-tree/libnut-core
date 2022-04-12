#include "../keypress.h"
#include "../deadbeef_rand.h"
#include "../microsleep.h"

#include <ctype.h> /* For isupper() */

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

#define toggleUniKey(c, down) toggleKey(c, down, MOD_NONE)

static void tapUniKey(char c)
{
	toggleUniKey(c, true);
	toggleUniKey(c, false);
}

void typeString(const char *str)
{
	unsigned short c;
	unsigned short c1;
	unsigned short c2;
	unsigned short c3;
	unsigned long n;

	while (*str != '\0')
	{
		c = *str++;

		// warning, the following utf8 decoder
		// doesn't perform validation
		if (c <= 0x7F)
		{
			// 0xxxxxxx one byte
			n = c;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			// 110xxxxx two bytes
			c1 = (*str++) & 0x3F;
			n = ((c & 0x1F) << 6) | c1;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			// 1110xxxx three bytes
			c1 = (*str++) & 0x3F;
			c2 = (*str++) & 0x3F;
			n = ((c & 0x0F) << 12) | (c1 << 6) | c2;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			// 11110xxx four bytes
			c1 = (*str++) & 0x3F;
			c2 = (*str++) & 0x3F;
			c3 = (*str++) & 0x3F;
			n = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
		}

		toggleUniKey(n, true);
		toggleUniKey(n, false);
	}
}

void typeStringDelayed(const char *str, const unsigned cpm)
{
	/* Characters per second */
	const double cps = (double)cpm / 60.0;

	/* Average milli-seconds per character */
	const double mspc = (cps == 0.0) ? 0.0 : 1000.0 / cps;

	while (*str != '\0')
	{
		tapUniKey(*str++);
		microsleep(mspc + (DEADBEEF_UNIFORM(0.0, 62.5)));
	}
}
