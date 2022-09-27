#include "../keycode.h"

MMKeyCode keyCodeForChar(const char c)
{
	return VkKeyScan(c);
}
