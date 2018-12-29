//
//  main.m
//  test_widnow
//
//  Created by George Watson on 01/07/2017.
//  Copyright © 2017 George Watson. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreVideo/CVDisplayLink.h>
#import <CoreServices/CoreServices.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
#import <mach/mach_time.h>

#define UPDATE_SHADER_LOC(X) (X##_loc = glGetUniformLocation(shader, #X))

static const char *vertex, *fragment;

static float vertices[] = {
   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
};

static unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3
};

static uint64_t start_mach;
mach_timebase_info_data_t mach_base_info;

static int eventModified = kFSEventStreamEventFlagItemFinderInfoMod |
                           kFSEventStreamEventFlagItemModified |
                           kFSEventStreamEventFlagItemInodeMetaMod |
                           kFSEventStreamEventFlagItemChangeOwner |
                           kFSEventStreamEventFlagItemXattrMod;

static int eventRenamed  = kFSEventStreamEventFlagItemCreated |
                           kFSEventStreamEventFlagItemRemoved |
                           kFSEventStreamEventFlagItemRenamed;

static int eventSystem   = kFSEventStreamEventFlagUserDropped |
                           kFSEventStreamEventFlagKernelDropped |
                           kFSEventStreamEventFlagEventIdsWrapped |
                           kFSEventStreamEventFlagHistoryDone |
                           kFSEventStreamEventFlagMount |
                           kFSEventStreamEventFlagUnmount |
                           kFSEventStreamEventFlagRootChanged;

const char* load_file(const char* path) {
  FILE *file = fopen(path, "rb");
  if (!file) {
    fprintf(stderr, "fopen \"%s\" failed: %d %s\n", path, errno, strerror(errno));
    exit(1);
  }
  
  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  rewind(file);
  
  char *data = (char*)calloc(length + 1, sizeof(char));
  fread(data, 1, length, file);
  fclose(file);
  
  return data;
}

GLuint make_shader(GLenum type, const char* src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nil);
  glCompileShader(shader);
  
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar *info = (GLchar*)calloc(length, sizeof(GLchar));
    glGetShaderInfoLog(shader, length, nil, info);
    fprintf(stderr, "glCompileShader failed:\n%s\n", info);
    
    free(info);
    abort();
  }
  
  return shader;
}

GLuint make_program(GLuint vert, GLuint frag) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);
  
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    GLchar *info = calloc(length, sizeof(GLchar));
    glGetProgramInfoLog(program, length, nil, info);
    fprintf(stderr, "glLinkProgram failed: %s\n", info);
    
    free(info);
    abort();
  }
  
  glDetachShader(program, vert);
  glDetachShader(program, frag);
  
  return program;
}

uint64_t get_ticks() {
  uint64_t now = mach_absolute_time();
  return (((now - start_mach) * mach_base_info.numer) / mach_base_info.denom) / 1000000;
}

static uint64_t old_time, current_time;
static float delta;

@interface OpenGLView : NSOpenGLView<NSWindowDelegate>
-(void)registerDisplayLink;
-(void)renderForTime:(CVTimeStamp)time;

-(void)windowWillClose:(NSNotification*)note;
-(void)windowDidResize:(NSNotification*)note;
-(void)mouseMoved:(NSEvent*)event;
-(BOOL)acceptsFirstResponder;

-(void)updateShaderLocs;
-(void)loadFragShader:(const char*)path;
@end

CVReturn displayCallback(CVDisplayLinkRef displayLink,
                         const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime,
                         CVOptionFlags flagsIn, CVOptionFlags *flagsOut,
                         void *displayLinkContext) {
  OpenGLView *view = (__bridge OpenGLView*)displayLinkContext;
  [view renderForTime: *inOutputTime];
  return kCVReturnSuccess;
}

@implementation OpenGLView {
  CVDisplayLinkRef displayLink;
  NSRect windowRect;
  GLuint shader, vert, frag, VAO, VBO, EBO,
         iResolution_loc, iTime_loc, iTimeDelta_loc,
         iFrame_loc, iFrameRate_loc, iMouse_loc;
}

-(id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format {
  self = [super initWithFrame:frameRect pixelFormat:format];
  
  [self windowDidResize:(nil)];
  windowRect.size = frameRect.size;
  glClearColor(220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.f);
  
  vertex = load_file("default.vert.glsl");
  fragment = load_file("default.frag.glsl");
  
  vert = make_shader(GL_VERTEX_SHADER, vertex);
  frag = make_shader(GL_FRAGMENT_SHADER, fragment);
  shader = make_program(vert, frag);
  
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  
  [self registerDisplayLink];
  return self;
}

-(void)registerDisplayLink
{
  CGDirectDisplayID displayID = CGMainDisplayID();
  CVReturn error = CVDisplayLinkCreateWithCGDisplay(displayID, &displayLink);
  NSAssert((kCVReturnSuccess == error), @"Creating Display Link error %d", error);
  
  error = CVDisplayLinkSetOutputCallback(displayLink, displayCallback, (__bridge void * _Nullable)(self));
  NSAssert((kCVReturnSuccess == error), @"Setting Display Link callback error %d", error);
  CVDisplayLinkStart(displayLink);
}

-(void)renderForTime:(CVTimeStamp)time {
  if ([self lockFocusIfCanDraw] == NO)
    return;
  
  old_time = current_time;
  current_time = get_ticks();
  delta = (float)(current_time - old_time) / 1000.0f;
  
  [[self openGLContext] makeCurrentContext];
  glClear(GL_COLOR_BUFFER_BIT);
  
  glUseProgram(shader);
  
  glUniform2f(iResolution_loc, windowRect.size.width, windowRect.size.height);
  glUniform1f(glGetUniformLocation(shader, "iGlobalTime"), current_time);
  glUniform1f(glGetUniformLocation(shader, "iDeltaTime"), delta);
  
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  
  glUseProgram(0);
  
  [[self openGLContext] flushBuffer];
  [self unlockFocus];
}

-(void)windowWillClose:(NSNotification*)notification {
  CVDisplayLinkStop(displayLink);
  [NSApp terminate:self];
}

-(void)windowDidResize:(NSNotification*)notification {
  NSSize size = [[_window contentView] frame].size;
  [[self openGLContext] makeCurrentContext];
  CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

  windowRect.size.width = size.width;
  windowRect.size.height = size.height;
  glViewport(0, 0, windowRect.size.width, windowRect.size.height);
  
  CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

-(BOOL)acceptsFirstResponder {
	return YES;
}

-(void)mouseMoved:(NSEvent*) event {
	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
	NSLog(@"Mouse pos: %lf, %lf", point.x, point.y);
}

-(void)updateShaderLocs {
  UPDATE_SHADER_LOC(iResolution);
  UPDATE_SHADER_LOC(iTime);
  UPDATE_SHADER_LOC(iTimeDelta);
  UPDATE_SHADER_LOC(iFrame);
  UPDATE_SHADER_LOC(iFrameRate);
  UPDATE_SHADER_LOC(iMouse);
}

-(void)loadFragShader:(const char*)path {
  free((char*)fragment);
  fragment = load_file(path);
  glDeleteShader(vert);
  glDeleteShader(frag);
  glDeleteProgram(shader);
  vert = make_shader(GL_VERTEX_SHADER, vertex);
  frag = make_shader(GL_FRAGMENT_SHADER, fragment);
  shader = make_program(vert, frag);
  [self updateShaderLocs];
}

-(void)dealloc {
  free((char*)vertex);
  free((char*)fragment);
  glDeleteProgram(shader);
  glDeleteShader(vert);
  glDeleteShader(frag);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
@end

static OpenGLView* glView;

void watcher_callback(ConstFSEventStreamRef stream,
                      void*  info,
                      size_t numEvents,
                      void*  paths,
                      const  FSEventStreamEventFlags flags[],
                      const  FSEventStreamEventId    ids[]) {
  char** _paths = (char**)paths;
  for(int i = 0; i < numEvents; i++) {
    printf("Changed: %s\nMofified: %d\nRenamed: %d\nSystem: %d\n",
           _paths[i],
           !!(flags[i] & eventModified),
           !!(flags[i] & eventRenamed),
           !!(flags[i] & eventSystem));
  }
}

int main(int argc, const char* argv[]) {
  @autoreleasepool {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    id menubar = [NSMenu alloc];
    id appMenuItem = [NSMenuItem alloc];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    id appMenu = [NSMenu alloc];
    id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
                                                 action:@selector(terminate:) keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    id window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 640, 480)
                                            styleMask:NSWindowStyleMaskTitled    |
                                                      NSWindowStyleMaskResizable |
                                                      NSWindowStyleMaskClosable  |
                                                      NSWindowStyleMaskMiniaturizable
                                              backing:NSBackingStoreBuffered
                                                defer:NO];
    [window center];
    [window setTitle: [[NSProcessInfo processInfo] processName]];
    
    NSOpenGLPixelFormatAttribute attribs[] = {
      NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
      NSOpenGLPFAColorSize, 24,
      NSOpenGLPFAAlphaSize, 8,
      NSOpenGLPFADoubleBuffer,
      NSOpenGLPFAAccelerated,
      NSOpenGLPFANoRecovery,
      NSOpenGLPFAClosestPolicy,
      0,
    };
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    
    glView = [[OpenGLView alloc] initWithFrame:[[window contentView] bounds]
                                   pixelFormat:pixelFormat];
    [glView loadFragShader:(argv[1])];
    
    CFStringRef path = CFStringCreateWithCString(nil, argv[1], kCFStringEncodingUTF8);
    CFArrayRef watcher = CFArrayCreate(nil, (const void**) &path, 1, nil);
    CFAbsoluteTime latency = 3.0;
    FSEventStreamRef stream;

    mach_timebase_info(&mach_base_info);
    start_mach = mach_absolute_time();
    current_time = get_ticks();
    
    stream = FSEventStreamCreate(nil,
                                 &watcher_callback,
                                 nil,
                                 watcher,
                                 kFSEventStreamEventIdSinceNow,
                                 latency,
                                 kFSEventStreamCreateFlagFileEvents);
    
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);

		[window setAcceptsMouseMovedEvents:YES];
    [window setContentView:glView];
    [window setDelegate:glView];
    
    [window makeKeyAndOrderFront:nil];
    [window display];
    
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
  }
  return 0;
}
