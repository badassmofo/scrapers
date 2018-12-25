//
//  main.c
//  spritesheet_gen
//
//  Created by Rory sprites[j]. Bellows on 19/11/2017.
//  Copyright © 2017 Rory B. Bellows. All rights reserved.
//

#include <stdio.h>
#include <png.h>
#define JSMN_STRICT
#include "3rdparty/jsmn.h"
#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rdparty/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdparty/stb_image_write.h"
#include <limits.h>
#include <libgen.h>

typedef struct {
  int w, h, x, y, fx, fy;
  char* src;
} img_t;

#define JSOSZ 7

static int jsoneq(const char* json, jsmntok_t* tok, const char* s) {
  return (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start && strncmp(json + tok->start, s, tok->end - tok->start) == 0);
}

#define JSEQ(x) jsoneq(test, &t[i + 1 + j], x)

#define SETINT(x) \
buf[0] = '\0'; \
sprintf(buf, "%.*s", t[i + 1 + j + 1].end - t[i + 1 + j + 1].start, test + t[i + 1 + j + 1].start); \
imgs[c].x = atoi(buf);

#define SETBOOL(x) \
buf[0] = '\0'; \
sprintf(buf, "%.*s", t[i + 1 + j + 1].end - t[i + 1 + j + 1].start, test + t[i + 1 + j + 1].start); \
imgs[c].x = !strcmp(buf, "true");

int main(int argc, const char * argv[]) {
  if (argc < 4) {
    fprintf(stderr, "ERROR: Invalid number of arguments, 3 required.\n");
    return 1;
  }
  const char* out_p = argv[1];
  int n_objs        = atoi(argv[2]);
  const char* test  = argv[3];
  printf("%s\n", test);
  
  printf("PARSING JSON...");
  jsmn_parser p;
  int32_t n_tokens = (n_objs * (JSOSZ * 2)) + n_objs + 1;
  jsmntok_t t[n_tokens];
  jsmn_init(&p);
  int r = jsmn_parse(&p, test, strlen(test), t, n_tokens);
  if (r < 0) {
    fprintf(stderr, "ERROR: Failed to parse JSON: %d\n", r);
    return 1;
  }
  if (r < 1 || t[0].type != JSMN_ARRAY) {
    fprintf(stderr, "ERROR: Array expected\n");
    return 1;
  }
  
  img_t imgs[n_objs];
  char buf[BUFSIZ];
  int c = 0, i = 0, j = 0;
  for (i = 1; i < r; i++) {
    if (t[i].type == JSMN_OBJECT) {
      if (t[i].size != JSOSZ) {
        fprintf(stderr, "ERROR: Object expected with size of 5: {x, y, width, height, flipX, flipY, src}\n");
        return 1;
      }
      for (j = 0; j < t[i].size * 2; j += 2) {
        if (JSEQ("src")) {
          asprintf(&imgs[c].src, "%.*s", t[i + 1 + j + 1].end - t[i + 1 + j + 1].start, test + t[i + 1 + j + 1].start);
        } else if (JSEQ("width"))  {
          SETINT(w);
        } else if (JSEQ("height")) {
          SETINT(h);
        } else if (JSEQ("x")) {
          SETINT(x);
        } else if (JSEQ("y")) {
          SETINT(y);
        } else if (JSEQ("fx")) {
          SETBOOL(fx);
        } else if (JSEQ("fy")) {
          SETBOOL(fy);
        } else {
          fprintf(stderr, "ERROR: Unexpected token: %.*s, %.*s\n",
                  t[i + 1 + j].end - t[i + 1 + j].start, test + t[i + 1 + j].start,
                  t[i + 1 + j + 1].end - t[i + 1 + j + 1].start, test + t[i + 1 + j + 1].start);
          return 1;
        }
      }
      i += t[i].size * 2;
      c++;
    } else {
      fprintf(stderr, "Object expected\n");
      return 1;
    }
  }
  puts("DONE!");
  
  int32_t left = INT_MAX, top = INT_MAX, right = INT_MIN, bottom = INT_MIN;
  for (int i = 0; i < n_objs; ++i) {
    if (imgs[i].x < left)
      left = imgs[i].x;
    if (imgs[i].y < top)
      top = imgs[i].y;
    if (imgs[i].x + imgs[i].w > right)
      right = imgs[i].x + imgs[i].w;
    if (imgs[i].y + imgs[i].h > bottom)
      bottom = imgs[i].y + imgs[i].h;
  }
  right += abs(left);
  bottom += abs(top);
  printf("CREATING IMAGE (%d, %d)...\n", right, bottom);
  
  size_t sz = right * bottom * 4;
  unsigned char* out = malloc(sz * sizeof(unsigned char));
  if (!out) {
    fprintf(stderr, "ERROR! Out of memeory\n");
    return 1;
  }
  memset(out, 0, sz);
  
  for (i = 0; i < n_objs; ++i) {
    printf("LOADING (%s)...", basename(imgs[i].src));
    int w, h, c;
    unsigned char* in = stbi_load(imgs[i].src, &w, &h, &c, 0);
    if (!in) {
      fprintf(stderr, "ERROR! Failed to load: %s (%s)\n", imgs[i].src, stbi_failure_reason());
      return 1;
    }
    puts("DONE!");
    
    if (w != imgs[i].w || h != imgs[i].h) {
      printf("RESIZING IMAGE (%d %d) to (%d %d)...", w, h, imgs[i].w, imgs[i].h);
      unsigned char* in_r = malloc(imgs[i].w * imgs[i].h * c * sizeof(unsigned char));
      if (!in_r) {
        fprintf(stderr, "ERROR! Out of memeory\n");
        return 1;
      }
      if (!stbir_resize(in, w, h, 0, in_r, imgs[i].w, imgs[i].h, 0, STBIR_TYPE_UINT8, c, (c == 4 ? 0 : STBIR_ALPHA_CHANNEL_NONE), (c == 4), STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT, STBIR_COLORSPACE_LINEAR, NULL)) {
        fprintf(stderr, "ERROR! Failed to resize: %s\n", imgs[i].src);
        return 1;
      }
      free(in);
      in = in_r;
      puts("DONE!");
    }
    
    if (imgs[i].fx) {
      printf("FLIPPING IMAGE (X)...");
      // TODO
      puts("DONE!");
    }
    
    if (imgs[i].fy) {
      printf("FLIPPING IMAGE (Y)...");
      int row;
      size_t bytes_per_row = (size_t)imgs[i].w * c * sizeof(unsigned char);
      unsigned char temp[2048];
      
      for (row = 0; row < (imgs[i].h >> 1); row++) {
        unsigned char* row0 = in + row * bytes_per_row;
        unsigned char* row1 = in + (imgs[i].h - row - 1) * bytes_per_row;
        
        size_t bytes_left = bytes_per_row;
        while (bytes_left) {
          size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
          memcpy(temp, row0, bytes_copy);
          memcpy(row0, row1, bytes_copy);
          memcpy(row1, temp, bytes_copy);
          row0 += bytes_copy;
          row1 += bytes_copy;
          bytes_left -= bytes_copy;
        }
      }
      puts("DONE!");
    }
    
    imgs[i].x += abs(left);
    imgs[i].y += abs(top);
    printf("WRITING: at (%d %d)...", imgs[i].x, imgs[i].y);
    
    for (int x = 0; x < imgs[i].w; ++x) {
      for (int y = 0; y < imgs[i].h; ++y) {
        unsigned char* p = out + ((imgs[i].x + x) + right * (imgs[i].y + y)) * 4;
        unsigned char* q = in + (x + imgs[i].w * y) * c;
        p[0] = q[0];
        p[1] = q[1];
        p[2] = q[2];
        p[3] = (c == 4 ? q[3] : 255);
      }
    }
    
    puts("DONE!");
    stbi_image_free(in);
  }
  puts("DONE!");
  
  printf("WRITING IMAGE...");  // TODO: out_p
  stbi_write_png("/Users/roryb/Desktop/test4.png", right, bottom, 4, out, 0);
  free(out);
  printf("ALL DONE! Bye!\n");
  
  return 0;
}
