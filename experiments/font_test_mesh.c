#include <stdio.h>
#include "3rdparty/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "font_test_helper.h"

static const int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;

static SDL_Window* window;
static SDL_GLContext context;
static FT_Library ft;

#undef GLAD_DEBUG

#ifdef GLAD_DEBUG
void pre_gl_call(const char *name, void *funcptr, int len_args, ...) {
  printf("Calling: %s (%d arguments)\n", name, len_args);
}
#endif

char* glGetError_str(GLenum err) {
  switch (err) {
    case GL_INVALID_ENUM:                  return "INVALID_ENUM"; break;
    case GL_INVALID_VALUE:                 return "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:             return "INVALID_OPERATION"; break;
    case GL_STACK_OVERFLOW:                return "STACK_OVERFLOW"; break;
    case GL_STACK_UNDERFLOW:               return "STACK_UNDERFLOW"; break;
    case GL_OUT_OF_MEMORY:                 return "OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION"; break;
    default:
      return "Unknown Error";
  }
}

void post_gl_call(const char *name, void *funcptr, int len_args, ...) {
  GLenum err = glad_glGetError();
  if (err != GL_NO_ERROR) {
    fprintf(stderr, "ERROR %d (%s) in %s\n", err, glGetError_str(err), name);
    abort();
  }
}

void cleanup() {
  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(context);
  FT_Done_FreeType(ft);
  printf("Goodbye!\n");
}

typedef struct {
  point_t size, bearing;
  GLuint advance, texture;
} font_texture_t;

typedef struct {
  FT_Face face;
  const char* name;
} font_t;

void init_font(font_t* f, const char* path, int size) {
  f->name = path;
  if (FT_New_Face(ft, path, 0, &f->face)) {
    fprintf(stderr, "Failed to load \"%s\"!\n", path);
    exit(-1);
  }
  FT_Set_Pixel_Sizes(f->face, 0, size);
}

void free_font(font_t* f) {
  FT_Done_Face(f->face);
}

int main(int argc, const char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Failed to initalize SDL!\n");
    return -1;
  }
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  
  SDL_GL_SetSwapInterval(1);
  
  window = SDL_CreateWindow(argv[0],
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
  if (!window) {
    fprintf(stderr, "Failed to create SDL window!\n");
    return -1;
  }
  
  context = SDL_GL_CreateContext(window);
  if (!context) {
    fprintf(stderr, "Failed to create OpenGL context!\n");
    return -1;
  }
  
  if (!gladLoadGL()) {
    fprintf(stderr, "Failed to load GLAD!\n");
    return -1;
  }
  
#ifdef GLAD_DEBUG
  glad_set_pre_callback(pre_gl_call);
#endif
  
  glad_set_post_callback(post_gl_call);
  
  printf("Vendor:   %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version:  %s\n", glGetString(GL_VERSION));
  printf("GLSL:     %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  
  if (FT_Init_FreeType(&ft)) {
    fprintf(stderr, "Failed to initalize FTLibrary!\n");
    return -1;
  }
  
  font_t test;
  init_font(&test, "/Library/Fonts/Times New Roman.ttf", 36);
  
  FT_UInt glyph_index = FT_Get_Char_Index(test.face, 'A');
  FT_Load_Glyph(test.face, glyph_index, FT_LOAD_NO_SCALE);
  char glyph_name[1024];
  FT_Get_Glyph_Name(test.face, glyph_index, glyph_name, 1024);
  
  printf("%s, %s %ld %ld\n%d: %s\n",
         test.face->family_name, test.face->style_name, test.face->num_faces, test.face->num_glyphs,
         glyph_index, glyph_name);
  
  FT_GlyphSlot slot = test.face->glyph;
  FT_Outline outline = slot->outline;
  FT_Glyph_Metrics metrics = slot->metrics;
  
  printf("%d %d\nContours / Points:\n", outline.n_points, outline.n_contours);
  
  for (int i = 0; i < outline.n_contours; i++)
    printf("%d ", outline.contours[i]);
  printf(" / ");
  for (int i = 0; i < outline.n_points; ++i)
    printf("(%ld, %ld) ", outline.points[i].x, outline.points[i].y * -1);
  printf("\n");
  
  return 0;
  
  mat4 projection;
  mat4_ortho(&projection, 0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, -1, 1);
  
  SDL_bool running = SDL_TRUE;
  SDL_Event e;
  
  Uint32 now = SDL_GetTicks();
  Uint32 then;
  float  delta;
  
  while (running) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          running = SDL_FALSE;
          break;
      }
    }
    
    then = now;
    now = SDL_GetTicks();
    delta = (float)(now - then) / 1000.0f;
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    glUseProgram(0);
    
    SDL_GL_SwapWindow(window);
  }
  
  return 0;
}

