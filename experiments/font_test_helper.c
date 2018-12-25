#include "font_test_helper.h"

char* __load_file(const char* path) {
  FILE *file = fopen(path, "rb");
  if (!file) {
    fprintf(stderr, "fopen \"%s\" failed: %d %s\n", path, errno, strerror(errno));
    abort();
  }
  
  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  rewind(file);
  
  char *data = (char*)calloc(length + 1, sizeof(char));
  fread(data, 1, length, file);
  fclose(file);
  
  return data;
}

GLuint __make_shader(GLenum type, const char* src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar *info = (GLchar*)calloc(length, sizeof(GLchar));
    glGetShaderInfoLog(shader, length, NULL, info);
    fprintf(stderr, "glCompileShader failed:\n%s\n", info);
    
    free(info);
    exit(-1);
  }
  
  return shader;
}

GLuint __make_program(GLuint vert, GLuint frag) {
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
    glGetProgramInfoLog(program, length, NULL, info);
    fprintf(stderr, "glLinkProgram failed: %s\n", info);
    
    free(info);
    exit(-1);
  }
  
  glDetachShader(program, vert);
  glDetachShader(program, frag);
  glDeleteShader(vert);
  glDeleteShader(frag);
  
  return program;
}

GLuint load_shader_str(const char* vert, const char* frag) {
  return __make_program(__make_shader(GL_VERTEX_SHADER,    vert),
                        __make_shader(GL_FRAGMENT_SHADER, frag));
}

GLuint load_shader_file(const char* _vert, const char* _frag) {
  const char* vert = __load_file(_vert);
  const char* frag = __load_file(_frag);
  
  GLuint ret = __make_program(__make_shader(GL_VERTEX_SHADER,    vert),
                              __make_shader(GL_FRAGMENT_SHADER, frag));
  free((char*)vert);
  free((char*)frag);
  
  return ret;
}

void mat4_zero(mat4* m) {
  m->xx = 0.0f;
  m->xy = 0.0f;
  m->xz = 0.0f;
  m->xw = 0.0f;
  
  m->yx = 0.0f;
  m->yy = 0.0f;
  m->yz = 0.0f;
  m->yw = 0.0f;
  
  m->zx = 0.0f;
  m->zy = 0.0f;
  m->zz = 0.0f;
  m->zw = 0.0f;
  
  m->wx = 0.0f;
  m->wy = 0.0f;
  m->wz = 0.0f;
  m->ww = 0.0f;
}

void mat4_id(mat4* m) {
  mat4_zero(m);
  m->xx = 1.0f;
  m->yy = 1.0f;
  m->zz = 1.0f;
  m->ww = 1.0f;
}

void mat4_ortho(mat4* m, float left, float right, float bottom, float top, float clip_near, float clip_far) {
  mat4_id(m);
  m->xx = 2.0f / (right - left);
  m->yy = 2.0f / (top - bottom);
  m->xw = - (right + left) / (right - left);
  m->yw = - (top + bottom) / (top - bottom);
  m->zz = - 2.0f / (clip_far - clip_near);
  m->zw = - (clip_far + clip_near) / (clip_far - clip_near);
}
