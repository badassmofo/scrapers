#include <stdio.h>
#include "3rdparty/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#define SLIM_HASH_IMPLEMENTATION
#include "3rdparty/slim_hash.h"
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

SH_GEN_DECL(dict, char, font_texture_t*);
SH_GEN_HASH_IMPL(dict, char, font_texture_t*);
typedef struct dict char_map;

typedef struct {
  FT_Face face;
  const char* name;
  char_map map;
} font_t;

void init_font(font_t* f, const char* path, int size) {
  f->name = path;
  dict_new(&f->map);
  if (FT_New_Face(ft, path, 0, &f->face)) {
    fprintf(stderr, "Failed to load \"%s\"!\n", path);
    exit(-1);
  }
  FT_Set_Pixel_Sizes(f->face, 0, size);
}

void load_char_font(font_t* f, char c) {
  if (!dict_contains(&f->map, c)) {
    if (FT_Load_Char(f->face, c, FT_LOAD_RENDER)) {
      fprintf(stderr, "Failed to load character '%c' from \"%s\"!\n", c, f->name);
      exit(-1);
    }
    
    font_texture_t* ch = (font_texture_t*)malloc(sizeof(font_texture_t));
    ch->size.x = f->face->glyph->bitmap.width;
    ch->size.y = f->face->glyph->bitmap.rows;
    ch->bearing.x = f->face->glyph->bitmap_left;
    ch->bearing.y = f->face->glyph->bitmap_top;
    ch->advance = (GLuint)f->face->glyph->advance.x;
    
    glGenTextures(1, &ch->texture);
    glBindTexture(GL_TEXTURE_2D, ch->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, f->face->glyph->bitmap.width, f->face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, f->face->glyph->bitmap.buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    dict_put(&f->map, c, ch);
  }
}

void load_char_range(font_t* f, char a, char b) {
  for (; a < b; ++a)
    load_char_font(f, a);
}
#define LOAD_DEF_CHAR_RANGE(F) load_char_range(F, ' ', '~');

void free_font(font_t* f) {
  FT_Done_Face(f->face);
  for(struct dict_slot* it = dict_start(&f->map); it != NULL; it = dict_next(&f->map, it)) {
    glDeleteTextures(1, &it->value->texture);
    free(it->value);
    dict_remove(&f->map, it);
  }
  dict_destroy(&f->map);
}

typedef struct {
  font_texture_t* texture;
  GLfloat verts[6][4];
} char_t;

typedef struct {
  font_t* font;
  GLuint VAO, VBO;
  const char* c_str;
  size_t length;
  char_t* chars;
} string_t;

void init_string(string_t* s, const char* text, font_t* f) {
  s->c_str = text;
  s->font = f;
  s->length = strlen(text);
  s->chars = (char_t*)calloc(s->length, sizeof(char_t));
  
  int x = 0, y = 0;
  for (int i = 0; i < s->length; ++i) {
    font_texture_t* tmp = dict_get(&f->map, text[i], NULL);
    if (!tmp) {
      load_char_font(f, text[i]);
      tmp = dict_get(&f->map, text[i], NULL);
    }
    
    GLfloat xpos = x + tmp->bearing.x;
    GLfloat ypos = y - (tmp->size.y - tmp->bearing.y);
    GLfloat w    = tmp->size.x;
    GLfloat h    = tmp->size.y;
    
    GLfloat verts[6][4] = {
      { xpos,     ypos + h,   0.0, 0.0 },
      { xpos,     ypos,       0.0, 1.0 },
      { xpos + w, ypos,       1.0, 1.0 },
      { xpos,     ypos + h,   0.0, 0.0 },
      { xpos + w, ypos,       1.0, 1.0 },
      { xpos + w, ypos + h,   1.0, 0.0 }
    };
    
    s->chars[i].texture = tmp;
    memcpy(&s->chars[i].verts, &verts, sizeof(s->chars[i].verts));
    
    x += (tmp->advance >> 6);
  }
  
  glGenVertexArrays(1, &s->VAO);
  glGenBuffers(1, &s->VBO);
  glBindVertexArray(s->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, s->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_string(string_t* s) {
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(s->VAO);
  
  for (int i = 0; i < s->length; ++i) {
    glBindTexture(GL_TEXTURE_2D, s->chars[i].texture->texture);
    glBindBuffer(GL_ARRAY_BUFFER, s->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(s->chars[i].verts), s->chars[i].verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
  
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void free_string(string_t* s) {
  free(s->chars);
  glDeleteVertexArrays(1, &s->VAO);
  glDeleteBuffers(1, &s->VBO);
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
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  if (FT_Init_FreeType(&ft)) {
    fprintf(stderr, "Failed to initalize FTLibrary!\n");
    return -1;
  }
  
  font_t test;
  init_font(&test, "/Library/Fonts/Times New Roman.ttf", 36);
  LOAD_DEF_CHAR_RANGE(&test);
  
  string_t str;
  init_string(&str, "SEE YOU SPACE COWBOY...", &test);
  
  GLuint shader = load_shader_file("font_test_quad.vert.glsl", "font_test_quad.frag.glsl");
  
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
    
    glUseProgram(shader);
    
    glUniform2f(glGetUniformLocation(shader, "position"), 10.f, 10.f);
    glUniform4f(glGetUniformLocation(shader, "color"), 1.f, 1.f, 1.f, 1.f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection.mat[0]);
    
    draw_string(&str);
    
    glUseProgram(0);
    
    SDL_GL_SwapWindow(window);
  }
  
  free_string(&str);
  free_font(&test);
  glDeleteProgram(shader);
  
  return 0;
}
