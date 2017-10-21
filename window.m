//
//  main.m
//  test
//
//  Created by George Watson on 25/06/2017.
//  Copyright © 2017 George Watson. All rights reserved.
//

#import <Cocoa/Cocoa.h>

int main(int argc, const char * argv[]) {
	@autoreleasepool {
		NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
		
		// Window bounds (x, y, width, height).
		NSRect windowRect = NSMakeRect(100, 100, 400, 400);
		NSWindow * window = [[NSWindow alloc] initWithContentRect:windowRect
																										styleMask:windowStyle
																											backing:NSBackingStoreBuffered
																												defer:NO];
		
		// Window controller:
		NSWindowController * windowController = [[NSWindowController alloc] initWithWindow:window];

		
		// TODO: Create app delegate to handle system events.
		// TODO: Create menus (especially Quit!)
		
		// Show window and run event loop.
		[window orderFrontRegardless];
		[NSApp run];
	}
	return 0;
}
