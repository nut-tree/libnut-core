#include "window.h"
#import <Cocoa/Cocoa.h>


Window::Window(int x, int y, int width, int height) {
    this->_x = x;
    this->_y = y;
    this->_width = width;
    this->_height = height;
}

void Window::show(int duration, float opacity) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];

    NSRect frame = NSMakeRect(this->_x, this->_y, this->_width, this->_height);
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