//
//  main.m
//  remote_mk
//
//  Created by Kirisame Marisa on 29/08/2017.
//  Copyright © 2017 Kirisame Marisa. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

static const CGEventMask mask = CGEventMaskBit(kCGEventLeftMouseDown) |
                                CGEventMaskBit(kCGEventLeftMouseUp) |
                                CGEventMaskBit(kCGEventRightMouseDown) |
                                CGEventMaskBit(kCGEventRightMouseUp) |
                                CGEventMaskBit(kCGEventLeftMouseDragged) |
                                CGEventMaskBit(kCGEventRightMouseDragged) |
                                CGEventMaskBit(kCGEventKeyDown) |
                                CGEventMaskBit(kCGEventKeyUp) |
                                CGEventMaskBit(kCGEventMouseMoved) |
                                CGEventMaskBit(kCGEventScrollWheel);

static CFMachPortRef event_tap = nil;

static CGPoint warp_point  = {0};

static int sockfd = 0;

#ifdef DEBUG
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define for_endian(size) for (int i = 0; i < size; ++i)
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define for_endian(size) for (int i = size - 1; i >= 0; --i)
#else
#error "Endianness not detected"
#endif

#define printb(value) ({ \
	typeof(value) _v = value; \
	__printb((typeof(_v) *) &_v, sizeof(_v)); \
})

void __printb(void *value, size_t size) {
	uint8_t byte;
	size_t blen = sizeof(byte) * 8;
	uint8_t bits[blen + 1];

	bits[blen] = '\0';
	for_endian((int)size) {
		byte = ((uint8_t *) value)[i];
		memset(bits, '0', blen);
		for (int j = 0; byte && j < blen; ++j) {
			if (byte & 0x80)
				bits[j] = '1';
			byte <<= 1;
		}
		printf("%s ", bits);
	}
}
#endif

CGEventRef event_cb(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *ref) {
	if (![NSApp isActive])
		return event;

	unsigned int event_mask = (int)CGEventMaskBit(type);
	int dx = 0, dy = 0, swdx = 0, swdy = 0;

	switch (type) {
		case kCGEventLeftMouseDown:
		case kCGEventLeftMouseUp:
		case kCGEventRightMouseDown:
		case kCGEventRightMouseUp:
		case kCGEventKeyUp:
			break;
		case kCGEventMouseMoved:
		case kCGEventLeftMouseDragged:
		case kCGEventRightMouseDragged:
			dx = (int)CGEventGetIntegerValueField(event, kCGMouseEventDeltaX);
			dy = (int)CGEventGetIntegerValueField(event, kCGMouseEventDeltaY);
			CGWarpMouseCursorPosition(warp_point);
			break;
		case kCGEventKeyDown:
			event_mask |= ((CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode) << 16);
			event_mask |= (CGEventGetFlags(event) << 24);
			break;
		 case kCGEventScrollWheel:
			event_mask = 0x1; // Default CGEventMaskBit value too long for int
			swdx = (int)CGEventGetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis2);
			swdy = (int)CGEventGetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis1);
			break;
		case kCGEventTapDisabledByUserInput:
		case kCGEventTapDisabledByTimeout:
			CGEventTapEnable(event_tap, true);
		default:
			return event;
	}

#ifdef DEBUG
	printf("debug: dx: %d, dy: %d, swxy: %d, swdy: %d\n\tevent: ", dx, dy, swdx, swdy);
	printb((short)(event_mask & 0xFFFFFFF));
	printf("key: ");
	printb((uint8_t)(event_mask >> 16));
	printf("mod: ");
	printb((uint8_t)(event_mask >> 24));
	printf("\n");
#endif

	char buf[256];
	sprintf(buf, "%d,%d,%d,%d,%d\n", dx, dx, swdx, swdy, event_mask);
	if (write(sockfd, buf, strlen(buf)) <= 0) 
		exit(0);

	return event;
}

@interface AppDelegate : NSApplication {}
@end

@implementation AppDelegate
-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApplication {
	(void)theApplication;
	return YES;
}
@end

@interface AppView : NSView {}
@end

@implementation AppView
-(id)initWithFrame:(NSRect)frame {
	if (self = [super initWithFrame:frame]) {}
	return self;
}

-(BOOL)acceptsFirstResponder {
	return YES;
}

-(BOOL)performKeyEquivalent:(NSEvent*)event {
	return YES;
}
@end

int main(int argc, const char* argv[]) {
	if (geteuid()) {
		NSLog(@"ERROR: Run as root");
		exit(-1);
	}

	struct addrinfo hints, *addr;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo("10.147.17.6", "1488", &hints, &addr);
	sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (connect(sockfd, addr->ai_addr, addr->ai_addrlen) < 0) {
		NSLog(@"ERROR: connecting to server");
		exit(-1);
	}

	CFRunLoopSourceRef loop = nil;
	@autoreleasepool {
		[NSApplication sharedApplication];
		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

		id menubar = [NSMenu alloc];
		id appMenuItem = [NSMenuItem alloc];
		[menubar addItem:appMenuItem];
		[NSApp setMainMenu:menubar];
		id appMenu = [NSMenu alloc];
		id appName = [[NSProcessInfo processInfo] processName];
		id quitTitle = [@"Quit" stringByAppendingString:appName];
		id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
                                                 action:@selector(terminate:)
                                          keyEquivalent:@"q"];
		[appMenu addItem:quitMenuItem];
		[appMenuItem setSubmenu:appMenu];

		id window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
                                            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                              backing:NSBackingStoreBuffered
                                                defer:NO];

		event_tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, mask, event_cb, NULL);
		loop = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);
		CFRunLoopAddSource(CFRunLoopGetCurrent(), loop, kCFRunLoopCommonModes);
		CGEventTapEnable(event_tap, true);

		[window center];
		[window setTitle: [[NSProcessInfo processInfo] processName]];
		[window makeKeyAndOrderFront:nil];

		id app_del = [AppDelegate alloc];
		if (!app_del)
			[NSApp terminate:nil];
		[NSApp setDelegate:app_del];
		id app_view = [[AppView alloc] initWithFrame:NSMakeRect(0, 0, 200, 200)];
		[window setContentView:app_view];

		NSScreen* screen = [NSScreen mainScreen];
		NSRect frame = [screen visibleFrame];
		warp_point = CGPointMake(frame.size.width / 2, frame.size.height / 2 - 200);
		CGWarpMouseCursorPosition(warp_point);
		[NSCursor hide];

		[NSApp activateIgnoringOtherApps:YES];
		[NSApp run];
	}

	close(sockfd);
	CFRelease(event_tap);
	CFRelease(loop);

	return 0;
}
