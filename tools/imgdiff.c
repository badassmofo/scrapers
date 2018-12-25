//
//  imgdiff.c
//  imgdiff
//
//  Created by Rory B. Bellows on 26/03/2018.
//  Copyright © 2018 Rory B. Bellows. All rights reserved.
//

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rdparty/stb_image_resize.h"
#include <stdio.h>
#include <unistd.h>

typedef struct {
  int* data, w, h;
} image_t;

#define RGB(r, g, b) (((unsigned int)r) << 16) | (((unsigned int)g) << 8) | b
#define XYSET(img, x, y, v) (img->data[(y) * img->w + (x)] = (v))
#define XYGET(img, x, y) (img->data[(y) * img->w + (x)])

image_t* load_image(const char* path, int re_w, int re_h) {
  if (access(path, F_OK) == -1) {
    fprintf(stderr, "ERROR! Invlaid filepath \"%s\"!\n", path);
    return NULL;
  }
  
  int w, h, c;
  unsigned char* data = stbi_load(path, &w, &h, &c, STBI_rgb);
  if (!data) {
    fprintf(stderr, "ERROR! Failed to load \"%s\"!\n", path);
    return NULL;
  }
  
  if ((re_w > 0 && re_h > 0) && (re_w != w || re_h != h)) {
    unsigned char* re_data = malloc(re_w * re_h * 3);
    stbir_resize_uint8(data, w, h, 0, re_data, re_w, re_h, 0, 3);
    stbi_image_free(data);
    data = re_data;
    w    = re_w;
    h    = re_h;
  }
  
  image_t* ret = malloc(sizeof(image_t));
  if (!ret) {
    fprintf(stderr, "ERROR! Out of memory!\n");
    return NULL;
  }
  size_t s = w * h * sizeof(unsigned int) + 1;
  ret->data = malloc(s);
  if (!ret->data) {
    fprintf(stderr, "ERROR! Out of memory!\n");
    return NULL;
  }
  memset(ret->data, 0, s);
  
  ret->w = w;
  ret->h = h;
  for (int x = 0; x < w; ++x) {
    for (int y = 0; y < h; ++y) {
      unsigned char* p = data + (x + h * y) * c;
      XYSET(ret, x, y, RGB(p[0], p[1], p[2]));
    }
  }
  
  stbi_image_free(data);
  return ret;
}

int main(int argc, const char * argv[]) {
  if (argc != 3) {
    fprintf(stderr, "ERROR! Invlaid number of arguments! Expected 2, got %d\n", argc - 1);
    return 1;
  }
  
  image_t *a = NULL, *b = NULL;
  if (!(a = load_image(argv[1], -1, -1)))
    return 2;
  if (!(b = load_image(argv[2], a->w, a->h))) {
    free(a->data);
    free(a);
    return 3;
  }
  
  double diff = 0.;
  for (int x = 0; x < a->w; ++x) {
    for (int y = 0; y < a->h; ++y) {
      int a_col = XYGET(a, x, y), b_col = XYGET(b, x, y);
      diff += fabs(((a_col >> 16) & 0xFF) - ((b_col >> 16) & 0xFF)) / 255.0;
      diff += fabs(((a_col >> 8)  & 0xFF) - ((b_col >> 8)  & 0xFF)) / 255.0;
      diff += fabs( (a_col        & 0xFF) -  (b_col        & 0xFF)) / 255.0;
    }
  }
  printf("%lf%%\n", 100. * diff / (double)(a->w * a->h * 3));
  
  if (a) {
    free(a->data);
    free(a);
  }
  if (b) {
    free(b->data);
    free(b);
  }
  return 0;
}
