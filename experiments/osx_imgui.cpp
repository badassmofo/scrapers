#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#include "3rdparty/imgui/imgui.h"

#define WINDOW_W 640
#define WINDOW_H 480

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

static struct imgui_values_t {
  float dpi_scale;
  bool m_pressed[2];
  float m_coords[2];
  unsigned int win_w, win_h, back_w, back_h;
  clock_t last_clock;
} vals = {
  .dpi_scale = 1.f,
  .m_pressed = { false, false },
  .m_coords = { 0, 0 },
  .win_w = WINDOW_W,
  .win_h = WINDOW_H,
  .back_w = 0,
  .back_h = 0
};

static bool map_keys(int* keymap) {
  if (*keymap == NSUpArrowFunctionKey)
    *keymap = ImGuiKey_LeftArrow;
  else if (*keymap == NSDownArrowFunctionKey)
    *keymap = ImGuiKey_DownArrow;
  else if (*keymap == NSLeftArrowFunctionKey)
    *keymap = ImGuiKey_LeftArrow;
  else if (*keymap == NSRightArrowFunctionKey)
    *keymap = ImGuiKey_RightArrow;
  else if (*keymap == NSHomeFunctionKey)
    *keymap = ImGuiKey_Home;
  else if (*keymap == NSEndFunctionKey)
    *keymap = ImGuiKey_End;
  else if (*keymap == NSDeleteFunctionKey)
    *keymap = ImGuiKey_Delete;
  else if (*keymap == 25) // SHIFT + TAB
    *keymap = 9; // TAB
  else
    return true;
  
  return false;
}

static void reset_keys() {
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[io.KeyMap[ImGuiKey_A]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_C]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_V]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_X]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_Y]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_Z]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_LeftArrow]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_RightArrow]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_Tab]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_UpArrow]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_DownArrow]] = false;
  io.KeysDown[io.KeyMap[ImGuiKey_Tab]] = false;
}

static void ImImpl_RenderDrawLists(ImDrawData* draw_data) {
  ImGuiIO& io = ImGui::GetIO();
  int fb_width = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
  int fb_height = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);
  if (fb_width == 0 || fb_height == 0)
    return;
  draw_data->ScaleClipRects(io.DisplayFramebufferScale);
  
  GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
  GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
  GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(draw_data->DisplayPos.x, draw_data->DisplayPos.x + draw_data->DisplaySize.x, draw_data->DisplayPos.y + draw_data->DisplaySize.y, draw_data->DisplayPos.y, -1.0f, +1.0f);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  ImVec2 pos = draw_data->DisplayPos;
  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
    const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
    glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, pos)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, uv)));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, col)));
    
    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
      const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback)
        pcmd->UserCallback(cmd_list, pcmd);
      else {
        ImVec4 clip_rect = ImVec4(pcmd->ClipRect.x - pos.x, pcmd->ClipRect.y - pos.y, pcmd->ClipRect.z - pos.x, pcmd->ClipRect.w - pos.y);
        if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f) {
          glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
          glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
          glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
        }
      }
      idx_buffer += pcmd->ElemCount;
    }
  }
  
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
  glPolygonMode(GL_FRONT, (GLenum)last_polygon_mode[0]); glPolygonMode(GL_BACK, (GLenum)last_polygon_mode[1]);
  glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
  glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void IMGUIExample_Draw(double elapsedMilliseconds)
{
  //vals.m_pressed[0] = vals.m_pressed[1] = false;
  ImGuiIO& io = ImGui::GetIO();
  // Setup resolution (every frame to accommodate for window resizing)
  int w,h;
  int display_w, display_h;
  display_w = vals.back_w;
  display_h = vals.back_h;
  w = vals.win_w;
  h = vals.win_h;
  vals.dpi_scale = vals.back_w / vals.win_w;
  // Display size, in pixels. For clamping windows positions.
  io.DisplaySize = ImVec2((float)display_w, (float)display_h);
  
  io.DeltaTime = elapsedMilliseconds /100.0; //convert in seconds
  
  // Setup inputs
  double mouse_x = 0, mouse_y = 0;
  mouse_x = vals.m_coords[0];
  mouse_y = vals.m_coords[1];
  io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
  io.MouseDown[0] = vals.m_pressed[0];
  io.MouseDown[1] = vals.m_pressed[1];
  
  ImGui::NewFrame();
  
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New")) {}
      if (ImGui::MenuItem("Open", "Ctrl+O")) {}
      if (ImGui::BeginMenu("Open Recent")) {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More..")) {
          ImGui::MenuItem("Hello");
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }
      if (ImGui::MenuItem("Save", "Ctrl+S")) {}
      if (ImGui::MenuItem("Save As..")) {}
      ImGui::Separator();
      if (ImGui::MenuItem("Quit", "Alt+F4"))
        [NSApp terminate:nil];
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
      if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "CTRL+X")) {}
      if (ImGui::MenuItem("Copy", "CTRL+C")) {}
      if (ImGui::MenuItem("Paste", "CTRL+V")) {}
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  
  ImGui::Begin("Sample window"); // begin window
  ImGui::End(); // end window
  
  // Rendering
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  glClearColor(220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui::Render();
}

@interface AppView : NSOpenGLView {
  NSTimer *anim_timer;
}
@end

@implementation AppView
-(void)anim_timer_fired:(NSTimer*)timer {
  [self setNeedsDisplay:YES];
}

-(id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format {
  self = [super initWithFrame:frameRect pixelFormat:format];
  if (self)
    vals.last_clock = clock();
  return self;
}

- (void)prepareOpenGL {
  [super prepareOpenGL];
  
#ifndef DEBUG
  GLint swapInterval = 1;
  [[self openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
  if (swapInterval == 0)
    NSLog(@"Error: Cannot set swap interval.");
#endif
}

- (void)drawView {
  NSWindow *mainWindow = [self window];
  NSPoint mousePosition = [mainWindow mouseLocationOutsideOfEventStream];
  
  mousePosition = [self convertPoint:mousePosition fromView:nil];
  vals.m_coords[0] = mousePosition.x;
  vals.m_coords[1] = mousePosition.y - 1.0f;
  
  clock_t thisclock = clock();
  unsigned long clock_delay = thisclock - vals.last_clock;
  double milliseconds = clock_delay * 1000.0f / CLOCKS_PER_SEC;
  IMGUIExample_Draw(milliseconds);
  vals.last_clock = thisclock;
  
  [[self openGLContext] flushBuffer];
  
  if (!anim_timer)
    anim_timer = [NSTimer scheduledTimerWithTimeInterval:0.017f target:self selector:@selector(anim_timer_fired:) userInfo:nil repeats:YES];
}

-(void)setViewportRect:(NSRect)bounds {
  vals.win_w = bounds.size.width;
  vals.win_h = bounds.size.height;
  
  if (vals.win_h == 0)
    vals.win_h = 1;
  
  glViewport(0, 0, vals.win_h, vals.win_h);
  vals.back_w = vals.win_w;
  vals.back_h = vals.win_h;
}

-(void)reshape {
  [self setViewportRect:self.bounds];
  [[self openGLContext] update];
  [self drawView];
}

-(void)drawRect:(NSRect)bounds {
  [self drawView];
}

#pragma mark -

-(BOOL)acceptsFirstResponder {
  return(YES);
}

-(BOOL)becomeFirstResponder {
  return(YES);
}

-(BOOL)resignFirstResponder {
  return(YES);
}

-(BOOL)isFlipped {
  return(YES);
}

-(void)keyUp:(NSEvent *)event {
  NSString *str = [event characters];
  ImGuiIO& io = ImGui::GetIO();
  int len = (int)[str length];
  for(int i = 0; i < len; i++) {
    int keymap = [str characterAtIndex:i];
    map_keys(&keymap);
    if(keymap < 512)
      io.KeysDown[keymap] = false;
  }
}

-(void)keyDown:(NSEvent *)event {
  NSString *str = [event characters];
  ImGuiIO& io = ImGui::GetIO();
  int len = (int)[str length];
  for(int i = 0; i < len; i++) {
    int keymap = [str characterAtIndex:i];
    if (map_keys(&keymap) && !io.KeyCtrl)
      io.AddInputCharacter(keymap);
      if (keymap < 512) {
        if(io.KeyCtrl)
          reset_keys();
        io.KeysDown[keymap] = true;
      }
  }
}

- (void)flagsChanged:(NSEvent *)event {
  unsigned int flags = [event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask;
  ImGuiIO& io = ImGui::GetIO();
  bool wasKeyShift = io.KeyShift;
  bool wasKeyCtrl  = io.KeyCtrl;
  io.KeyShift      = flags & NSEventModifierFlagShift;
  io.KeyCtrl       = flags & NSEventModifierFlagCommand;
  bool keyShiftReleased = wasKeyShift && !io.KeyShift;
  bool keyCtrlReleased  = wasKeyCtrl  && !io.KeyCtrl;
  if(keyShiftReleased || keyCtrlReleased)
    reset_keys();
}

-(void)mouseDown:(NSEvent *)event {
  if (ImGui::GetIO().KeyShift)
    [super mouseDown:event];
  else {
    int button = (int)[event buttonNumber];
    vals.m_pressed[button] = true;
  }
}

-(void)mouseUp:(NSEvent *)event {
  int button = (int)[event buttonNumber];
  vals.m_pressed[button] = false;
  [super mouseUp:event];
}

- (void)scrollWheel:(NSEvent *)event {
  double deltaX, deltaY;
  
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
  if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6) {
    deltaX = [event scrollingDeltaX];
    deltaY = [event scrollingDeltaY];
    
    if ([event hasPreciseScrollingDeltas]) {
      deltaX *= 0.1;
      deltaY *= 0.1;
    }
  }
  else
#endif /*MAC_OS_X_VERSION_MAX_ALLOWED*/
  {
    deltaX = [event deltaX];
    deltaY = [event deltaY];
  }
  
  if (fabs(deltaX) > 0.0 || fabs(deltaY) > 0.0) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += deltaY * 0.1f;
  }
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic, readonly) NSWindow *window;
@end

@implementation AppDelegate
@synthesize window = _window;

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)app {
  (void)app;
  return YES;
}

- (NSWindow*)window {
  if (_window != nil)
    return _window;
  
  _window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, WINDOW_W, WINDOW_H)
                                       styleMask:NSWindowStyleMaskResizable | NSWindowStyleMaskTitled | NSWindowStyleMaskFullSizeContentView
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
  if (!_window) {
    fprintf(stderr, "alloc() failed: out of memory\n");
    [NSApp terminate:nil];
  }
  
  [_window center];
  [_window setTitle:@""];
  [_window makeKeyAndOrderFront:nil];
  [_window setMovableByWindowBackground:YES];
  [_window setTitlebarAppearsTransparent:YES];
  [[_window standardWindowButton:NSWindowZoomButton] setHidden:YES];
  [[_window standardWindowButton:NSWindowCloseButton] setHidden:YES];
  [[_window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
  
  return _window;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  id menubar = [NSMenu alloc];
  id appMenuItem = [NSMenuItem alloc];
  [menubar addItem:appMenuItem];
  [NSApp setMainMenu:menubar];
  id appMenu = [NSMenu alloc];
  id quitTitle = [@"Quit " stringByAppendingString:[[NSProcessInfo processInfo] processName]];
  id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
                                               action:@selector(terminate:)
                                        keyEquivalent:@"q"];
  [appMenu addItem:quitMenuItem];
  [appMenuItem setSubmenu:appMenu];
  
  NSOpenGLPixelFormatAttribute attrs[] = {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFADepthSize, 32,
    0
  };
  
  NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
  id view = [[AppView alloc] initWithFrame:self.window.frame pixelFormat:format];
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
  if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
    [view setWantsBestResolutionOpenGLSurface:NO];
#endif
  [self.window setContentView:view];
  if ([view openGLContext] == nil) {
    NSLog(@"No OpenGL Context!");
    [NSApp terminate:nil];
  }
  
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Tab] = 9;
  io.KeyMap[ImGuiKey_LeftArrow] = ImGuiKey_LeftArrow;
  io.KeyMap[ImGuiKey_RightArrow] = ImGuiKey_RightArrow;
  io.KeyMap[ImGuiKey_UpArrow] = ImGuiKey_UpArrow;
  io.KeyMap[ImGuiKey_DownArrow] = ImGuiKey_DownArrow;
  io.KeyMap[ImGuiKey_Home] = ImGuiKey_Home;
  io.KeyMap[ImGuiKey_End] = ImGuiKey_End;
  io.KeyMap[ImGuiKey_Delete] = ImGuiKey_Delete;
  io.KeyMap[ImGuiKey_Backspace] = 127;
  io.KeyMap[ImGuiKey_Enter] = 13;
  io.KeyMap[ImGuiKey_Escape] = 27;
  io.KeyMap[ImGuiKey_A] = 'a';
  io.KeyMap[ImGuiKey_C] = 'c';
  io.KeyMap[ImGuiKey_V] = 'v';
  io.KeyMap[ImGuiKey_X] = 'x';
  io.KeyMap[ImGuiKey_Y] = 'y';
  io.KeyMap[ImGuiKey_Z] = 'z';
  io.DeltaTime = 1.0f/60.0f;
  io.RenderDrawListsFn = ImImpl_RenderDrawLists;
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
  GLuint tex_id;
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);
  io.Fonts->TexID = (void *)(intptr_t)tex_id;
  
  ImGui::StyleColorsClassic();
}
@end

int main(int argc, const char * argv[]) {
  @autoreleasepool {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    id app_del = [[AppDelegate alloc] init];
    if (!app_del) {
      fprintf(stderr, "alloc() failed: out of memory\n");
      [NSApp terminate:nil];
    }
    [NSApp setDelegate:app_del];

    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
  }
}
