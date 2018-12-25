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

typedef struct __async_node {
  jmp_buf jmp;
  struct __async_node* next;
} node_t;
static node_t *jmps = NULL, *last = NULL;

node_t* __async_append_jmp() {
  node_t* new = malloc(sizeof(node_t));
  new->next = NULL;
  if (!jmps)
    jmps = new;
  else {
    node_t* tmp = jmps;
    while (tmp->next)
      tmp = tmp->next;
    tmp->next = new;
  }
  last = new;
  return new;
}

node_t* __async_get_next() {
  node_t* tmp = (last->next ? last->next : jmps);
  last = tmp;
  return tmp;
}

void __async_remove_jmp(node_t* n) {
  if (last == n)
    last = (last->next ? last->next : jmps);
  if (n == jmps) {
    if (jmps->next) {
      node_t* tmp = jmps->next;
      free(jmps);
      jmps = tmp;
    } else {
      free(jmps);
      jmps = NULL;
      last = NULL;
      return;
    }
  } else {
    node_t* cursor = jmps;
    while (cursor->next != n)
      cursor = cursor->next;
    node_t* tmp = cursor->next;
    cursor->next = tmp->next;
    free(tmp);
  }
}

void foo() {
  node_t* a = __async_append_jmp();
  if (setjmp(a->jmp) == 0)
    longjmp(__async_get_next()->jmp, 1);
  while (true) {
    printf("foo");
    longjmp(__async_get_next()->jmp, 1);
  }
}

void bar(int seconds) {
  node_t* a = __async_append_jmp();
  if (setjmp(a->jmp) == 0)
    longjmp(__async_get_next()->jmp, 1);
  time_t t = time(NULL) + seconds, ct;
  while ((ct = time(NULL)) < t) {
    printf("bar");
    longjmp(__async_get_next()->jmp, 1);
  }
}

//int main(int argc, const char* argv[]) {
//  node_t* a = __async_append_jmp();
//  if (setjmp(a->jmp) == 0)
//    foo();
//  if (setjmp(a->jmp) == 0)
//    bar(5);
//  return 0;
//}

