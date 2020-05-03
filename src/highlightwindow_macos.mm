#include "highlightwindow.h"
#import <Cocoa/Cocoa.h>

void showHighlightWindow(int x, int y, int width, int height, int duration, float opacity) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];

    NSRect frame = NSMakeRect(x, y, width, height);
    NSUInteger styleMask = NSWindowStyleMaskBorderless;
    NSWindow * window =  [[[NSWindow alloc] initWithContentRect:frame styleMask:styleMask backing:NSBackingStoreBuffered defer:NO] autorelease];
    [window setBackgroundColor:[NSColor redColor]];
    [window setAlphaValue:opacity];
    [window orderFrontRegardless];

    [NSTimer scheduledTimerWithTimeInterval: duration
                repeats: NO
                block: ^(NSTimer * timer){
                [NSApp stop:0];
                NSRect f = [window frame];
                f.size = CGSizeMake(0, 0);
                [window setFrame: f display: YES animate: NO];
            }];
    [NSApp run];
    [pool release];
}