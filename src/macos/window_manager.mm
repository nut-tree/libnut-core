#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <ApplicationServices/ApplicationServices.h>
#include "../window_manager.h"

std::vector<int64_t> getWindows() {
  CGWindowListOption listOptions = kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  std::vector<int64_t> windowHandles;

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

MMRect getWindowRect(const int64_t windowHandle) {
  CGWindowListOption listOptions = kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  std::vector<std::string> windowNames;

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *ownerPid = info[(id)kCGWindowOwnerPID];
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    auto app = [NSRunningApplication runningApplicationWithProcessIdentifier: [ownerPid intValue]];

    if (app && [windowNumber intValue] == windowHandle) {
      CGRect windowRect;
      if (CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)info[(id)kCGWindowBounds], &windowRect)) {
        return MMRectMake(windowRect.origin.x, windowRect.origin.y, windowRect.size.height, windowRect.size.width);
      }
        return MMRectMake(0, 0, 0, 0);
    }
  }

  if (windowList) {
    CFRelease(windowList);
  }

  return MMRectMake(0, 0, 0, 0);
}

std::string getWindowTitle(const int64_t windowHandle) {
  CGWindowListOption listOptions = kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
  CFArrayRef windowList = CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

  std::vector<std::string> windowNames;

  for (NSDictionary *info in (NSArray *)windowList) {
    NSNumber *ownerPid = info[(id)kCGWindowOwnerPID];
    NSNumber *windowNumber = info[(id)kCGWindowNumber];

    auto app = [NSRunningApplication runningApplicationWithProcessIdentifier: [ownerPid intValue]];

    if (app && [windowNumber intValue] == windowHandle) {
      NSString *windowName = info[(id)kCGWindowName];
      return [windowName UTF8String];
    }
  }

  if (windowList) {
    CFRelease(windowList);
  }

  return "";
}