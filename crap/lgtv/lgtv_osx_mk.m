//
//  main.m
//  input_capture
//
//  Created by Rory B. Bellows on 28/01/2018.
//  Copyright © 2018 Rory B. Bellows. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <Foundation/Foundation.h>
#import <ApplicationServices/ApplicationServices.h>
#import <Cocoa/Cocoa.h>
#include <signal.h>

static volatile int running = 1;
void ctrlc(int dummy) {
  running = 0;
}

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

static CFDataRef kb_data = NULL;
static const UCKeyboardLayout* kb_layout = NULL;

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
  int dx = 0, dy = 0, swdx = 0, swdy = 0, key = 0, mod = 0;
  char *key_str = NULL, *mod_str = NULL;
  
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
    case kCGEventKeyDown: {
      UInt16 action = 0;
      UInt32 state = 0, dead_key_state = 0;
      UniCharCount actual_len = 0;
      UniChar unistr[255];
      memset(unistr, 0x0, sizeof(unistr));
      
      key = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
      UCKeyTranslate(kb_layout, key, action, state, LMGetKbdType(), 0, &dead_key_state, 255, &actual_len, unistr);
      NSString* ns_key_str = [[NSString stringWithCharacters:unistr length:(NSUInteger)actual_len] lowercaseString];
      key_str = (char*)[ns_key_str UTF8String];
      
      mod = CGEventGetFlags(event);
      NSMutableString* ns_mod_str = [NSMutableString string];
      if (!!(mod & kCGEventFlagMaskControl) == YES)
        [ns_mod_str appendString:@"CTRL:"];
      if (!!(mod & kCGEventFlagMaskAlternate) == YES)
        [ns_mod_str appendString:@"ALT:"];
      if (!!(mod & kCGEventFlagMaskCommand) == YES)
        [ns_mod_str appendString:@"CMD:"];
      if (!!(mod & kCGEventFlagMaskShift) == YES)
        [ns_mod_str appendString:@"SHIFT:"];
      if (!!(mod & kCGEventFlagMaskAlphaShift) == YES)
        [ns_mod_str appendString:@"CAPSLOCK:"];
      mod_str = (char*)[ns_mod_str UTF8String];
      break;
    }
    case kCGEventScrollWheel:
      event_mask = 1;
      swdx = (int)CGEventGetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis2);
      swdy = (int)CGEventGetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis1);
      break;
    case kCGEventTapDisabledByUserInput:
    case kCGEventTapDisabledByTimeout:
      CGEventTapEnable(event_tap, true);
    default:
      return event;
  }
  
  printf("%d,%d,%d,%d,%d,%d,%s,%d,%s\n", dx, dy, swdx, swdy, event_mask, key, key_str, mod, mod_str);
  fflush(stdout);
  
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

int main(int argc, const char * argv[]) {
  if (geteuid()) {
    NSLog(@"ERROR: Run as root");
    exit(-1);
  }
  
  kb_data = (CFDataRef)TISGetInputSourceProperty(TISCopyCurrentKeyboardInputSource(), kTISPropertyUnicodeKeyLayoutData);
  kb_layout = (const UCKeyboardLayout*)CFDataGetBytePtr(kb_data);
  
  signal(SIGINT, ctrlc);
  
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
    
    NSRect win_size = NSMakeRect(0, 0, 200, 200);
    id window = [[NSWindow alloc] initWithContentRect:win_size
                                            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                              backing:NSBackingStoreBuffered
                                                defer:NO];
    
    event_tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, mask, event_cb, NULL);
    loop = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), loop, kCFRunLoopCommonModes);
    CGEventTapEnable(event_tap, true);
    
    [window center];
    [window setTitle:@"LGTV Controller"];
    [window makeKeyAndOrderFront:nil];
    [window setMovableByWindowBackground:YES];
    [window setTitlebarAppearsTransparent:YES];
    [[window standardWindowButton:NSWindowZoomButton] setHidden:YES];
    [[window standardWindowButton:NSWindowCloseButton] setHidden:NO];
    [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
    
    id app_del = [AppDelegate alloc];
    if (!app_del)
      [NSApp terminate:nil];
    [NSApp setDelegate:app_del];
    id app_view = [[AppView alloc] initWithFrame:win_size];
    [window setContentView:app_view];
    
    NSScreen* screen = [NSScreen mainScreen];
    NSRect frame = [screen visibleFrame];
    warp_point = CGPointMake(frame.size.width / 2, (frame.size.height / 2) - (win_size.size.height / 2));
    CGWarpMouseCursorPosition(warp_point);
    [NSCursor hide];
    
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
  }
  
  CFRelease(event_tap);
  CFRelease(loop);
  return 0;
}
