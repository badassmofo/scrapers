//
//  window_osx.m
//  ido
//
//  Created by Rusty Shackleford on 01/03/2014.
//  Copyright (c) 2014 Rusty Shackleford. All rights reserved.
//

#include "window.h"
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#import <Cocoa/Cocoa.h>
using namespace ido;

#ifdef TRACE_ENABLE
#define trace printf
#else
#include <stdarg.h>
inline void trace(const char* f, ...)
{
}
#endif

@interface skMacDelegate : NSObject// < NSApplicationDelegate >
/* Example: Fire has the same problem no explanation */
{
}
@end

@implementation skMacDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
{
  return YES;
}
@end


@interface skOpenGLWindow : NSWindow
{
  window_t* m_ownerWin;
}
@end

@implementation skOpenGLWindow
- (id) initWithContentRect: (NSRect)rect styleMask:(NSUInteger)wndStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferFlg win:(window_t*)ownerWin
{
  self = [super initWithContentRect:rect styleMask:wndStyle backing:bufferingType defer:deferFlg];
  m_ownerWin = ownerWin;
  
  [self setAcceptsMouseMovedEvents:YES];
  [self makeFirstResponder:self];
  
  trace("%s\n",__FUNCTION__);
  return self;
}

- (void) resizeGL
{
  trace("%s\n",__FUNCTION__);
}

- (void) windowDidResize: (NSNotification *)notification
{
  trace("%s\n",__FUNCTION__);
}

- (void) windowWillClose: (NSNotification *)notification
{
  trace("%s\n",__FUNCTION__);
  [NSApp terminate:nil];	// This can also be exit(0);
}

@end


@interface skOpenGLView : NSOpenGLView
{
  window_t* m_ownerWin;
  int m_width;
  int m_height;
  NSTrackingRectTag _tag;
  bool m_isMouseCursorIn;
}
@end

@implementation skOpenGLView
-(void) setOwnerWindow: (window_t*) win
{
  m_isMouseCursorIn = false;
  m_ownerWin = win;
}

- (BOOL)canBecomeKeyView
{
  return YES;
}

- (BOOL)acceptsFirstResponder
{
  return YES;
}

@end

  
  void skAddMenu(void)
  {
    
    NSMenu *mainMenu;
    
    mainMenu=[NSMenu alloc];
    [mainMenu initWithTitle:@"Minimum"];
    
    NSMenuItem *fileMenu;
    fileMenu=[[NSMenuItem alloc] initWithTitle:@"File" action:NULL keyEquivalent:[NSString string]];
    [mainMenu addItem:fileMenu];
    
    NSMenu *fileSubMenu;
    fileSubMenu=[[NSMenu alloc] initWithTitle:@"File"];
    [fileMenu setSubmenu:fileSubMenu];
    
    NSMenuItem *fileMenu_Quit;
    fileMenu_Quit=[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    [fileMenu_Quit setTarget:NSApp];
    [fileSubMenu addItem:fileMenu_Quit];
    
    [NSApp setMainMenu:mainMenu];
    
  }
  

window_t::window_t(unsigned int width, unsigned int height, const std::string& title) {
  
  [NSApplication sharedApplication];
  
  // Set Front process (need for keyboard and mouse events at console mode)
  ProcessSerialNumber psn = { 0, kCurrentProcess };
  TransformProcessType(&psn, kProcessTransformToForegroundApplication);
  SetFrontProcess(&psn);
  
  [NSApp finishLaunching];
  
  
  NSRect contRect;
  contRect = NSMakeRect(x, y, width, height);
  
  unsigned int winStyle =
  NSTitledWindowMask
  //NSBorderlessWindowMask
  | NSClosableWindowMask
  | NSMiniaturizableWindowMask
  | NSResizableWindowMask
  | NSWindowFullScreenButton;
  
  window = [skOpenGLWindow alloc];
  [window
   initWithContentRect:contRect
   styleMask:winStyle
   backing:NSBackingStoreBuffered
   defer:NO];
  [window setTitle:[NSString stringWithUTF8String:title.c_str()]];
  
  // for CloseButton
  skMacDelegate *delegate;
  delegate = [skMacDelegate alloc];
  [delegate init];
  [NSApp setDelegate: delegate];
  
  
  NSOpenGLPixelFormat *format;
  NSOpenGLPixelFormatAttribute formatAttrib[] =
  {
    NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)32,
    NSOpenGLPFAStencilSize, 1,
    NSOpenGLPFADoubleBuffer, 1,
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
    0
  };
  
  format=[NSOpenGLPixelFormat alloc];
  [format initWithAttributes: formatAttrib];
  
  context = [skOpenGLView alloc];
  [context setOwnerWindow:this];
  contRect = NSMakeRect( 0, 0, width, height);
  [context
   initWithFrame:contRect
   pixelFormat:format];
  
  	[[context openGLContext] makeCurrentContext];
  [window setContentView:context];
  [window makeFirstResponder:context];
  
  [window makeKeyAndOrderFront:nil];
  [window makeMainWindow];
  
  NSRect rc = [context bounds];
  
  [NSApp activateIgnoringOtherApps:YES];
  
  skAddMenu();
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  
  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);
  
  // Shader sources
  const GLchar* vertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "void main() {"
  "   gl_Position = vec4(position, 0.0, 1.0);"
  "}";
  const GLchar* fragmentSource =
  "#version 150 core\n"
  "out vec4 outColor;"
  "void main() {"
  "   outColor = vec4(1.0, 1.0, 1.0, 1.0);"
  "}";
  
  GLfloat vertices[] = {
    0.0f, 0.5f,
    0.5f, -0.5f,
    -0.5f, -0.5f
  };
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  
  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  
  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
  
  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  
  while (1) {
    glClear( GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    [[context openGLContext] flushBuffer];
  }
}

window_t::~window_t() {
  
}
