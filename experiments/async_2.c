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

typedef struct __node {
  jmp_buf jmp;
  struct __node* next;
} node_t;
static node_t *jmp, *last;

void __async_add_jmp(node_t* n) {
  node_t* tmp = jmp;
  while (tmp->next)
    tmp = tmp->next;
  tmp->next = n;
}

void __async_del_jmp(node_t* n) {
  node_t* tmp = jmp;
  while (tmp != n)
    tmp = tmp->next;
  // do something
}

node_t* __async_next() {
  node_t* tmp = (last && last->next ? last->next : jmp);
  last = tmp;
  return tmp;
}

void foo() {
  node_t* jmp = malloc(sizeof(node_t*));
  jmp->next = NULL;
  __async_add_jmp(jmp);
  if (setjmp(jmp->jmp) == 0)
    longjmp(__async_next()->jmp, 1);
  while (true) {
    printf("foo");
    longjmp(__async_next()->jmp, 1);
  }
}

void async_wait(int seconds) {
  time_t t = time(NULL) + seconds, ct;
  while ((ct = time(NULL)) < t)
    longjmp(__async_next()->jmp, 1);
}

//int main(int argc, const char* argv[]) {
//  if (!jmp) {
//    jmp = malloc(sizeof(node_t));
//    jmp->next = NULL;
//  }
//  
//  if (setjmp(jmp->jmp) == 0)
//    foo();
//  
//  if (setjmp(jmp->jmp) == 0)
//    async_wait(5);
//  
//  return 0;
//}

