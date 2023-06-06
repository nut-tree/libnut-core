#include "../highlightwindow.h"
#import <Cocoa/Cocoa.h>

void showHighlightWindow(int32_t x, int32_t y, int32_t width, int32_t height, long duration, float opacity) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
    bool appIsRunning = [NSApp isRunning];
    NSRect mainScreenFrameRect = [[NSScreen mainScreen] frame];
    CGFloat screenHeight = mainScreenFrameRect.size.height;
    NSRect frame = NSMakeRect(x, screenHeight - y - height, width, height);
    NSUInteger styleMask = NSWindowStyleMaskBorderless;
    NSWindow *window = [[[NSWindow alloc] initWithContentRect:frame styleMask:styleMask backing:NSBackingStoreBuffered defer:NO] autorelease];
    [window setBackgroundColor:[NSColor redColor]];
    [window setAlphaValue:opacity];
    [window orderFrontRegardless];

    [NSTimer scheduledTimerWithTimeInterval:duration / 1000.0
                                    repeats:NO
                                      block:^(NSTimer *timer) {
                                          if (!appIsRunning) {
                                              [NSApp stop:nil];
                                          }
                                          NSRect f = [window frame];
                                          f.size = CGSizeMake(0, 0);
                                          [window setFrame:f display:YES animate:NO];
                                      }];
    if (!appIsRunning) {
        [NSApp run];
    }
    [pool release];
}