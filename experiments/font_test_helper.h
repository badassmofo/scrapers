#ifndef helper_h
#define helper_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "3rdparty/glad.h"

#define GLSL(VERSION,CODE) "#version " #VERSION "\n" #CODE

GLuint load_shader_str(const char*, const char*);
GLuint load_shader_file(const char*, const char*);

typedef union {
  struct {
    float x, y;
  };
  float xy[2];
} point_t;

typedef union {
  struct {
    float xx; float yx; float zx; float wx;
    float xy; float yy; float zy; float wy;
    float xz; float yz; float zz; float wz;
    float xw; float yw; float zw; float ww;
  };
  float mat[16];
} mat4;

void mat4_zero(mat4*);
void mat4_id(mat4*);
void mat4_ortho(mat4*, float, float, float, float, float, float);

#endif /* helper_h */
