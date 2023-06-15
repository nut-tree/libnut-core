#include "../window_manager.h"
#import <AppKit/AppKit.h>
#import <AppKit/NSAccessibility.h>
#import <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CGWindow.h>
#import <Foundation/Foundation.h>

NSDictionary *getWindowInfo(int64_t windowHandle) {
  CGWindowListOption listOptions =
      kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList =
      CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    if (windowHandle == [windowNumber intValue]) {
      CFRetain(info);
      CFRelease(windowList);
      return info;
    }
  }

  if (windowList) {
    CFRelease(windowList);
  }

  return nullptr;
}

WindowHandle getActiveWindow() {
  CGWindowListOption listOptions =
      kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList =
      CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *ownerPid = info[(id)kCGWindowOwnerPID];
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    auto app = [NSRunningApplication
        runningApplicationWithProcessIdentifier:[ownerPid intValue]];

    if (![app isActive]) {
      continue;
    }

    CFRelease(windowList);
    return [windowNumber intValue];
  }

  if (windowList) {
    CFRelease(windowList);
  }
  return -1;
}

std::vector<WindowHandle> getWindows() {
  CGWindowListOption listOptions =
      kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList =
      CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  std::vector<WindowHandle> windowHandles;

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *ownerPid = info[(id)kCGWindowOwnerPID];
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    auto app = [NSRunningApplication
        runningApplicationWithProcessIdentifier:[ownerPid intValue]];
    auto path = app ? [app.bundleURL.path UTF8String] : "";

    if (app && path != "") {
      windowHandles.push_back([windowNumber intValue]);
    }
  }

  if (windowList) {
    CFRelease(windowList);
  }

  return windowHandles;
}

MMRect getWindowRect(const WindowHandle windowHandle) {
  auto windowInfo = getWindowInfo(windowHandle);
  if (windowInfo != nullptr && windowHandle >= 0) {
    CGRect windowRect;
    if (CGRectMakeWithDictionaryRepresentation(
            (CFDictionaryRef)windowInfo[(id)kCGWindowBounds], &windowRect)) {
      return MMRectMake(windowRect.origin.x, windowRect.origin.y,
                        windowRect.size.width, windowRect.size.height);
    }
  }
  return MMRectMake(0, 0, 0, 0);
}

std::string getWindowTitle(const WindowHandle windowHandle) {
  auto windowInfo = getWindowInfo(windowHandle);
  if (windowInfo != nullptr && windowHandle >= 0) {
    NSString *windowName = windowInfo[(id)kCGWindowName];
    return std::string(
        [windowName UTF8String],
        [windowName lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
  }
  return "";
}

bool focusWindow(const WindowHandle windowHandle) {
  CGWindowListOption listOptions =
      kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList =
      CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  bool activated = false;

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *ownerPid = info[(id)kCGWindowOwnerPID];
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    if ([windowNumber intValue] == windowHandle) {
      NSRunningApplication *app = [NSRunningApplication
          runningApplicationWithProcessIdentifier:[ownerPid intValue]];
      [app activateWithOptions:NSApplicationActivateIgnoringOtherApps];
      activated = true;
    }
  }

  if (windowList) {
    CFRelease(windowList);
  }

  return activated;
}

/*
  This function takes an input windowhandle (a kCGWindowNumber) and a rect (size
  & origin) and resizes the window to the given rect.
*/
bool resizeWindow(const WindowHandle windowHandle, const MMRect rect) {

  NSDictionary *windowInfo = getWindowInfo(windowHandle);
  if (windowInfo == nullptr || windowHandle < 0) {
    NSLog(@"Could not find window info for window handle %lld", windowHandle);
    return false;
  }

  pid_t pid = [[windowInfo objectForKey:(id)kCGWindowOwnerPID] intValue];
  AXUIElementRef app = AXUIElementCreateApplication(pid);
  AXUIElementRef window;
  AXError error = AXUIElementCopyAttributeValue(app, kAXFocusedWindowAttribute,
                                                (CFTypeRef *)&window);

  if (error == kAXErrorSuccess) {
    AXValueRef positionValue = AXValueCreate((AXValueType)kAXValueCGPointType,
                                             (const void *)&rect.origin);

    // extract the size from the rect

    CGSize size = CGSizeMake(rect.size.width, rect.size.height);
    AXValueRef sizeValue =
        AXValueCreate((AXValueType)kAXValueCGSizeType, (const void *)&size);

    AXUIElementSetAttributeValue(window, kAXPositionAttribute, positionValue);
    AXUIElementSetAttributeValue(window, kAXSizeAttribute, sizeValue);

    // log the position and size of the window

    CFRelease(positionValue);
    CFRelease(sizeValue);
    CFRelease(window);
    CFRelease(app);

    return true;
  } else {
    NSLog(@"Could not resize window with window handle %lld", windowHandle);
    CFRelease(app);
    return false;
  }

  return YES;
}
