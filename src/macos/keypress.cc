#include "../keypress.h"
#include "../microsleep.h"
#include "../deadbeef_rand.h"

#include <cctype>

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
    // TODO: Add operator overloads for MMKeyFlags
    if (code == K_META) {
      flags = static_cast<MMKeyFlags>(flags | MOD_META);
    }
    if (code == K_ALT) {
      flags = static_cast<MMKeyFlags>(flags | MOD_ALT);
    }
    if (code == K_CONTROL) {
      flags = static_cast<MMKeyFlags>(flags | MOD_CONTROL);
    }
    if (code == K_SHIFT || code == K_RIGHTSHIFT) {
      flags = static_cast<MMKeyFlags>(flags | MOD_SHIFT);
    }

    MMKeyFlags activeKeyFlags;
    if (down) {
      activeKeyFlags = static_cast<MMKeyFlags>(flags | flagBuffer);
      flagBuffer = static_cast<MMKeyFlags>(flagBuffer | activeKeyFlags);
    } else {
      activeKeyFlags = static_cast<MMKeyFlags>(flags ^ flagBuffer);
      flagBuffer = static_cast<MMKeyFlags>(flagBuffer ^ flags);
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
    flags = static_cast<MMKeyFlags>(flags | MOD_SHIFT); /* Not sure if this is safe for all layouts. */
  }

  toggleKeyCode(keyCode, down, flags);
}

void tapKey(char c, MMKeyFlags flags) {
  toggleKey(c, true, flags);
  toggleKey(c, false, flags);
}

void typeString(const std::u16string &str) {
  CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  for (auto it = str.begin(); it != str.end(); ++it) {
    UniChar c = *it;
    CGEventRef downEvent = CGEventCreateKeyboardEvent(src, 0, true);
    CGEventKeyboardSetUnicodeString(downEvent, 1, &c);
    CGEventPost(kCGHIDEventTap, downEvent);

    CGEventRef upEvent = CGEventCreateKeyboardEvent(src, 0, false);
    CGEventKeyboardSetUnicodeString(upEvent, 1, &c);
    CGEventPost(kCGHIDEventTap, upEvent);

    CFRelease(downEvent);
    CFRelease(upEvent);
		microsleep(DEADBEEF_RANDRANGE(1, 10));
  }
  CFRelease(src);
}

