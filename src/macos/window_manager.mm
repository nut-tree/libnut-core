#include <CoreGraphics/CGWindow.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include "../window_manager.h"

NSDictionary* getWindowInfo(int64_t windowHandle) {
  CGWindowListOption listOptions = kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

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
  CGWindowListOption listOptions = kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *ownerPid = info[(id)kCGWindowOwnerPID];
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    auto app = [NSRunningApplication runningApplicationWithProcessIdentifier: [ownerPid intValue]];

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
  CGWindowListOption listOptions = kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  std::vector<WindowHandle> windowHandles;

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *ownerPid = info[(id)kCGWindowOwnerPID];
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    auto app = [NSRunningApplication runningApplicationWithProcessIdentifier: [ownerPid intValue]];
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
    if (CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)windowInfo[(id)kCGWindowBounds], &windowRect)) {
      return MMRectMake(windowRect.origin.x, windowRect.origin.y, windowRect.size.width, windowRect.size.height);
    }
  }
  return MMRectMake(0, 0, 0, 0);
}

std::string getWindowTitle(const WindowHandle windowHandle) {
  auto windowInfo = getWindowInfo(windowHandle);
  if (windowInfo != nullptr && windowHandle >= 0) {
    NSString *windowName = windowInfo[(id)kCGWindowName];
    return std::string([windowName UTF8String], [windowName lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
  }
  return "";
}

BOOL focusWindow(NSWindow *window) {
    if (window) {
        // Restore the window if it's minimized
        if ([window isMiniaturized]) {
            [window deminiaturize:nil];
        }
        
        // Try to set the window to the foreground
        [window makeKeyAndOrderFront:nil];
        
        return YES;
    }
    return NO;
}

BOOL resizeWindow(NSWindow *window, int width, int height) {
    if (window) {
        NSRect frame = [window frame];
        frame.size = NSMakeSize(width, height);
        [window setFrame:frame display:YES animate:NO];
        return YES;
    }
    return NO;
}