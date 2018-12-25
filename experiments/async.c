//
//  main.c
//  async_test
//
//  Created by Rory B. Bellows on 15/10/2017.
//  Copyright © 2017 Rory B. Bellows. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
#include <time.h>

//  int       max_iteration, iter;
//
//  jmp_buf   Main, PointA, PointB;
//
//  void      Ping(void);
//  void      Pong(void);
//
//  void  main(int  argc, char* argv[])
//  {
//    max_iteration = abs(atoi(argv[1]));
//    iter = 1;
//    if (setjmp(Main) == 0)
//      Ping();
//    if (setjmp(Main) == 0)
//      Pong();
//    longjmp(PointA, 1);
//  }
//
//  void  Ping(void)
//  {
//    if (setjmp(PointA) == 0)
//      longjmp(Main, 1);
//    while (1) {
//      printf("%3d : Ping-", iter);
//      if (setjmp(PointA) == 0)
//        longjmp(PointB, 1);
//    }
//  }
//
//  void  Pong(void)
//  {
//    if (setjmp(PointB) == 0)
//      longjmp(Main, 1);
//    while (1) {
//      printf("Pong\n");
//      iter++;
//      if (iter > max_iteration)
//        exit(0);
//      if (setjmp(PointB) == 0)
//        longjmp(PointA, 1);
//    }
//  }

typedef struct {
  jmp_buf main;
  int length;
  jmp_buf* jmps;
} async_ctx;

int __async_ctx_resize(async_ctx* ctx, int size) {
  if (!ctx)
    return -1;
  
  jmp_buf* new = realloc(ctx->jmps, size * sizeof(void*));
  if (!new)
    return -1;
  ctx->jmps = new;
  
  return 1;
}

#define async(fn) ({ \
  if (setjmp(ctx.main) == 0) \
    fn; \
})

#define coroutine(fn, ...) __attribute__((noinline)) fn { \
  async_ctx ctx; \
  ctx.length = 0; \
  ctx.jmps   = malloc(sizeof(jmp_buf)); \
  if (!ctx.jmps) { \
    puts("FATAL: malloc() failed."); \
    exit(EXIT_FAILURE); \
  } \
  __VA_ARGS__ \
  free(ctx.jmps); \
}

coroutine(int main(int argc, const char* argv[]), {
  return 0;
})
