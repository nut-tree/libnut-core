#include "../window_manager.h"
#import <AppKit/AppKit.h>
#import <AppKit/NSAccessibility.h>
#import <ApplicationServices/ApplicationServices.h>
#import <CoreGraphics/CGWindow.h>
#import <Foundation/Foundation.h>

NSDictionary *getWindowInfo(int64_t windowHandle) {
    CGWindowListOption listOptions =
            kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
    CFArrayRef windowList =
            CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

    for (NSDictionary *info in (NSArray *) windowList) {
        NSNumber *windowNumber = info[(id) kCGWindowNumber];

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

AXUIElementRef getWindowByHandle(const WindowHandle windowHandle, pid_t *outPid) {
    // Collect list of on-screen windows
    CGWindowListOption listOptions =
            kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
    CFArrayRef windowList =
            CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

    AXUIElementRef targetWindow = nullptr;
    // Look for matching window
    for (NSDictionary *info in (NSArray *) windowList) {
        NSNumber *windowNumber = info[(id) kCGWindowNumber];
        if ([windowNumber intValue] == windowHandle) {
            *outPid = [info[(id) kCGWindowOwnerPID] intValue];
            AXUIElementRef app = AXUIElementCreateApplication(*outPid);

            CFArrayRef windowArray;
            AXError error = AXUIElementCopyAttributeValue(app, kAXWindowsAttribute, (CFTypeRef *) &windowArray);
            if (error == kAXErrorSuccess) {
                CFIndex count = CFArrayGetCount(windowArray);
                for (CFIndex i = 0; i < count; i++) {
                    auto window = static_cast<AXUIElementRef>(CFArrayGetValueAtIndex(windowArray, i));
                    // Your logic here to match the window with the handle
                    // Set targetWindow = window if it matches
                }
                CFRelease(windowArray);
            }
            CFRelease(app);
            break;
        }
    }

    if (windowList) {
        CFRelease(windowList);
    }

    return targetWindow;
}

WindowHandle getActiveWindow() {
    CGWindowListOption listOptions =
            kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
    CFArrayRef windowList =
            CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

    for (NSDictionary *info in (NSArray *) windowList) {
        NSNumber *ownerPid = info[(id) kCGWindowOwnerPID];
        NSNumber *windowNumber = info[(id) kCGWindowNumber];

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

    for (NSDictionary *info in (NSArray *) windowList) {
        NSNumber *ownerPid = info[(id) kCGWindowOwnerPID];
        NSNumber *windowNumber = info[(id) kCGWindowNumber];

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
                (CFDictionaryRef) windowInfo[(id) kCGWindowBounds], &windowRect)) {
            return MMRectMake(windowRect.origin.x, windowRect.origin.y,
                              windowRect.size.width, windowRect.size.height);
        }
    }
    return MMRectMake(0, 0, 0, 0);
}

std::string getWindowTitle(const WindowHandle windowHandle) {
    auto windowInfo = getWindowInfo(windowHandle);
    if (windowInfo != nullptr && windowHandle >= 0) {
        NSString *windowName = windowInfo[(id) kCGWindowName];
        return {
                [windowName UTF8String],
                [windowName lengthOfBytesUsingEncoding:NSUTF8StringEncoding]
        };
    }
    return "";
}

/**
 * Focuses on the window provided via its handle.
 *
 * This function collects a list of on-screen windows and matches the
 * windowHandle with their window numbers. If found, the corresponding
 * application is brought to foreground. The function then uses accessibility
 * APIs to specifically focus the target window using its title.
 *
 * @param windowHandle Handle to the window that needs to be focused.
 *
 * @return bool If the function executes without any errors, it returns true.
 *              If it can't retrieve window information or windowHandle is
 *              invalid, it returns false.
 */
bool focusWindow(const WindowHandle windowHandle) {

    // Collect list of on-screen windows
    CGWindowListOption listOptions =
            kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements;
    CFArrayRef windowList =
            CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);

    // Look for matching window and bring application to foreground
    for (NSDictionary *info in (NSArray *) windowList) {
        NSNumber *ownerPid = info[(id) kCGWindowOwnerPID];
        NSNumber *windowNumber = info[(id) kCGWindowNumber];
        if ([windowNumber intValue] == windowHandle) {
            NSRunningApplication *app = [NSRunningApplication
                    runningApplicationWithProcessIdentifier:[ownerPid intValue]];
            [app activateWithOptions:NSApplicationActivateIgnoringOtherApps];
        }
    }

    // Clean up window list
    if (windowList) {
        CFRelease(windowList);
    }

    // Retrieve window info
    NSDictionary *windowInfo = getWindowInfo(windowHandle);
    if (windowInfo == nullptr || windowHandle < 0) {
        // NSLog(@"Could not find window info for window handle %lld", windowHandle);
        return false;
    }

    // Create application object for accessibility
    pid_t pid = [[windowInfo objectForKey:(id) kCGWindowOwnerPID] intValue];
    AXUIElementRef app = AXUIElementCreateApplication(pid);

    // Get target window title
    NSString *targetWindowTitle = [windowInfo objectForKey:(id) kCGWindowName];

    CFArrayRef windowArray;
    AXError error = AXUIElementCopyAttributeValue(app, kAXWindowsAttribute,
                                                  (CFTypeRef *) &windowArray);

    // Iterate through windows to find target and bring it to front
    if (error == kAXErrorSuccess) {
        CFIndex count = CFArrayGetCount(windowArray);
        for (CFIndex i = 0; i < count; i++) {
            auto window =
                    (AXUIElementRef) CFArrayGetValueAtIndex(windowArray, i);

            CFTypeRef windowTitle;
            AXUIElementCopyAttributeValue(window, kAXTitleAttribute, &windowTitle);
            if (windowTitle && CFGetTypeID(windowTitle) == CFStringGetTypeID()) {
                auto title = (__bridge NSString *) windowTitle;
                if ([title isEqualToString:targetWindowTitle]) {
                    AXUIElementPerformAction(window, kAXRaiseAction);
                    break;
                }
            }

            // Clean up window title
            if (windowTitle) {
                CFRelease(windowTitle);
            }
        }

        // Clean up window array
        CFRelease(windowArray);
    } else {
        // NSLog(@"Failed to retrieve the window array.");
    }

    // Clean up application object
    CFRelease(app);

    // Successfully executed
    return true;
}

/**
 * Resizes the window provided via its handle to the specified width/height.
 *
 * @param windowHandle Handle to the window that needs to be resized.
 * @param newSize The width/height to which the window should be resized.
 *
 * @return bool True if the window is successfully resized, false otherwise.
 */
bool resizeWindow(const WindowHandle windowHandle, const MMSize newSize) {

    // Retrieve window info
    NSDictionary *windowInfo = getWindowInfo(windowHandle);
    if (windowInfo == nullptr || windowHandle < 0) {
        // NSLog(@"Could not find window info for window handle %lld", windowHandle);
        return false;
    }

    // Create application object for accessibility
    pid_t pid = [[windowInfo objectForKey:(id) kCGWindowOwnerPID] intValue];
    AXUIElementRef app = AXUIElementCreateApplication(pid);
    AXUIElementRef window;

    AXError error = AXUIElementCopyAttributeValue(app, kAXFocusedWindowAttribute,
                                                  (CFTypeRef *) &window);

    // If no error occurred, proceed with the resize and reposition operations
    if (error == kAXErrorSuccess) {

        CGSize size = CGSizeMake(newSize.width, newSize.height);
        AXValueRef sizeValue =
                AXValueCreate((AXValueType) kAXValueCGSizeType, (const void *) &size);

        // Set new position and size
        AXUIElementSetAttributeValue(window, kAXSizeAttribute, sizeValue);

        // Clean up AXValue and AXUIElement objects
        CFRelease(sizeValue);
        CFRelease(window);
        CFRelease(app);

        // Return true to indicate successful resize
        return true;
    } else {
        // NSLog(@"Could not resize window with window handle %lld", windowHandle);
        CFRelease(app);
        return false;
    }

    return YES;
}

/**
 * Moves the window provided via its handle to the specified point.
 *
 * @param windowHandle Handle to the window that needs to be resized.
 * @param newOrigin The x/y coordinates to which the window should be repositioned.
 *
 * @return bool True if the window is successfully repositioned, false otherwise.
 */
bool moveWindow(const WindowHandle windowHandle, const MMPoint newOrigin) {

    // Retrieve window info
    NSDictionary *windowInfo = getWindowInfo(windowHandle);
    if (windowInfo == nullptr || windowHandle < 0) {
        // NSLog(@"Could not find window info for window handle %lld", windowHandle);
        return false;
    }

    // Create application object for accessibility
    pid_t pid = [[windowInfo objectForKey:(id) kCGWindowOwnerPID] intValue];
    AXUIElementRef app = AXUIElementCreateApplication(pid);
    AXUIElementRef window;

    AXError error = AXUIElementCopyAttributeValue(app, kAXFocusedWindowAttribute, (CFTypeRef *) &window);

    // If no error occurred, proceed with the resize and reposition operations
    if (error == kAXErrorSuccess) {

        // Create AXValue objects for position and size
        CGPoint point = CGPointMake(newOrigin.x, newOrigin.y);
        AXValueRef positionValue = AXValueCreate((AXValueType) kAXValueCGPointType,
                                                 (const void *) &point);

        // Set new position and size
        AXUIElementSetAttributeValue(window, kAXPositionAttribute, positionValue);

        // Clean up AXValue and AXUIElement objects
        CFRelease(positionValue);
        CFRelease(window);
        CFRelease(app);

        // Return true to indicate successful resize
        return YES;
    } else {
        // NSLog(@"Could not resize window with window handle %lld", windowHandle);
        CFRelease(app);
        return false;
    }

    return YES;
}
