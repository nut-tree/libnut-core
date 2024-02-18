#include "../keypress.h"
#include "../deadbeef_rand.h"
#include "../microsleep.h"

#include <ctype.h> /* For isupper() */

#include <ApplicationServices/ApplicationServices.h>
#import <IOKit/hidsystem/IOHIDLib.h>
#import <IOKit/hidsystem/ev_keymap.h>

MMKeyFlags flagBuffer;

static io_connect_t _getAuxiliaryKeyDriver(void) {
  static mach_port_t sEventDrvrRef = 0;
  mach_port_t masterPort, service, iter;
  kern_return_t kr;

  if (!sEventDrvrRef) {
    kr = IOMasterPort(bootstrap_port, &masterPort);
    assert(KERN_SUCCESS == kr);
    kr = IOServiceGetMatchingServices(
        masterPort, IOServiceMatching(kIOHIDSystemClass), &iter);
    assert(KERN_SUCCESS == kr);
    service = IOIteratorNext(iter);
    assert(service);
    kr = IOServiceOpen(service, mach_task_self(), kIOHIDParamConnectType,
                       &sEventDrvrRef);
    assert(KERN_SUCCESS == kr);
    IOObjectRelease(service);
    IOObjectRelease(iter);
  }
  return sEventDrvrRef;
}

void toggleKeyCode(MMKeyCode code, const bool down, MMKeyFlags flags) {
  /* The media keys all have 1000 added to them to help us detect them. */
  if (code >= 1000) {
    code = code - 1000; /* Get the real keycode. */
    NXEventData event;
    kern_return_t kr;
    IOGPoint loc = {0, 0};
    UInt32 evtInfo = code << 16 | (down ? NX_KEYDOWN : NX_KEYUP) << 8;
    bzero(&event, sizeof(NXEventData));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = evtInfo;
    kr = IOHIDPostEvent(_getAuxiliaryKeyDriver(), NX_SYSDEFINED, loc, &event,
                        kNXEventDataVersion, 0, FALSE);
    assert(KERN_SUCCESS == kr);
  } else {
    CGEventSourceRef src =
        CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    CGEventRef keyEvent =
        CGEventCreateKeyboardEvent(src, (CGKeyCode)code, down);
    assert(keyEvent != NULL);

    // Check if keycode is one of the available modifier keys and set keyflags
    // accordingly
    if (code == K_META || code == K_RIGHTMETA) {
      flags |= MOD_META;
    }
    if (code == K_ALT || code == K_RIGHTALT) {
      flags |= MOD_ALT;
    }
    if (code == K_CONTROL || code == K_RIGHTCONTROL) {
      flags |= MOD_CONTROL;
    }
    if (code == K_SHIFT || code == K_RIGHTSHIFT) {
      flags |= MOD_SHIFT;
    }
    if (code == K_FUNCTION) {
      flags |= MOD_FN;
    }

    MMKeyFlags activeKeyFlags;
    if (down) {
      activeKeyFlags = flags | flagBuffer;
      flagBuffer |= activeKeyFlags;
    } else {
      activeKeyFlags = flags ^ flagBuffer;
      flagBuffer ^= flags;
    }

    CGEventSetFlags(keyEvent, activeKeyFlags);
    CGEventPost(kCGHIDEventTap, keyEvent);
    CFRelease(keyEvent);
    CFRelease(src);
  }
}

void tapKeyCode(MMKeyCode code, MMKeyFlags flags) {
  toggleKeyCode(code, true, flags);
  toggleKeyCode(code, false, flags);
}

void toggleKey(char c, const bool down, MMKeyFlags flags) {
  MMKeyCode keyCode = keyCodeForChar(c);

  if (isupper(c) && !(flags & MOD_SHIFT)) {
    flags |= MOD_SHIFT; /* Not sure if this is safe for all layouts. */
  }

  toggleKeyCode(keyCode, down, flags);
}

void tapKey(char c, MMKeyFlags flags) {
  toggleKey(c, true, flags);
  toggleKey(c, false, flags);
}

void toggleUnicodeKey(unsigned long ch, const bool down) {
  /* This function relies on the convenient
   * CGEventKeyboardSetUnicodeString(), which allows us to not have to
   * convert characters to a keycode, but does not support adding modifier
   * flags. It is therefore only used in typeString() and typeStringDelayed()
   * -- if you need modifier keys, use the above functions instead. */
  CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  CGEventRef keyEvent = CGEventCreateKeyboardEvent(src, 0, down);
  if (keyEvent == NULL) {
    fputs("Could not create keyboard event.\n", stderr);
    return;
  }

  if (ch > 0xFFFF) {
    // encode to utf-16 if necessary
    unsigned short surrogates[] = {0xD800 + ((ch - 0x10000) >> 10),
                                   0xDC00 + (ch & 0x3FF)};

    CGEventKeyboardSetUnicodeString(keyEvent, 2, &surrogates);
  } else {
    CGEventKeyboardSetUnicodeString(keyEvent, 1, &ch);
  }

  CGEventPost(kCGHIDEventTap, keyEvent);
  CFRelease(keyEvent);
  CFRelease(src);
}

void toggleUniKey(char c, const bool down) { toggleUnicodeKey(c, down); }

static void tapUniKey(char c) {
  toggleUniKey(c, true);
  toggleUniKey(c, false);
}

void typeString(const char *str) {
  unsigned short c;
  unsigned short c1;
  unsigned short c2;
  unsigned short c3;
  unsigned long n;

  while (*str != '\0') {
    c = *str++;

    // warning, the following utf8 decoder
    // doesn't perform validation
    if (c <= 0x7F) {
      // 0xxxxxxx one byte
      n = c;
    } else if ((c & 0xE0) == 0xC0) {
      // 110xxxxx two bytes
      c1 = (*str++) & 0x3F;
      n = ((c & 0x1F) << 6) | c1;
    } else if ((c & 0xF0) == 0xE0) {
      // 1110xxxx three bytes
      c1 = (*str++) & 0x3F;
      c2 = (*str++) & 0x3F;
      n = ((c & 0x0F) << 12) | (c1 << 6) | c2;
    } else if ((c & 0xF8) == 0xF0) {
      // 11110xxx four bytes
      c1 = (*str++) & 0x3F;
      c2 = (*str++) & 0x3F;
      c3 = (*str++) & 0x3F;
      n = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
    }

    toggleUnicodeKey(n, true);
    toggleUnicodeKey(n, false);
  }
}

void typeStringDelayed(const char *str, const unsigned cpm) {
  /* Characters per second */
  const double cps = (double)cpm / 60.0;

  /* Average milli-seconds per character */
  const double mspc = (cps == 0.0) ? 0.0 : 1000.0 / cps;

  while (*str != '\0') {
    tapUniKey(*str++);
    microsleep(mspc + (DEADBEEF_UNIFORM(0.0, 62.5)));
  }
}
