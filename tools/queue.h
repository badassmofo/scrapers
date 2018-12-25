#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "threads.h"
#include <stdlib.h>

typedef struct _q_node {
  void*  data;
  size_t size;
  struct _q_node* next;
} queue_node_t;

typedef struct _queue {
  queue_node_t* nodes;
  size_t length;
  mtx_t mod_lock, read_lock;
} queue_t;

void queue_init(queue_t*);
void queue_add(queue_t*, void*, size_t);
queue_node_t* queue_get(queue_t*);
void free_queue_node(queue_node_t*);
void free_queue(queue_t*);

#if defined QUEUE_IMPL
void queue_init(queue_t* q) {
  q->length = 0;
  q->nodes  = NULL;

  mtx_init(&q->mod_lock, mtx_plain);
  mtx_init(&q->read_lock, mtx_plain);
  mtx_lock(&q->read_lock);
}

void queue_add(queue_t* q, void* _data, size_t _size) {
  queue_node_t* qn = calloc(1, sizeof(queue_node_t));
  if (!qn)
    return;

  qn->data = _data;
  qn->size = _size;

  mtx_lock(&q->mod_lock);
  qn->next = q->nodes;
  q->nodes = qn;
  q->length++;

  mtx_unlock(&q->mod_lock);
  mtx_unlock(&q->read_lock);
}

queue_node_t* queue_get(queue_t* q) {
  queue_node_t *qn, *pqn;

  mtx_lock(&q->mod_lock);
  mtx_lock(&q->read_lock);

  if (!(qn = pqn = q->nodes)) {
    mtx_unlock(&q->mod_lock);
    return (queue_node_t*)NULL;
  }

  qn = pqn = q->nodes;
  while ((qn->next)) {
    pqn = qn;
    qn  = qn->next;
  }

  pqn->next = NULL;
  if (q->length <= 1)
    q->nodes = NULL;
  q->length--;

  if (q->length > 0)
    mtx_unlock(&q->read_lock);
  mtx_unlock(&q->mod_lock);

  return qn;
}

void free_queue_node(queue_node_t* qn) {
  free(qn->data);
}

void free_queue(queue_t* q) {
  free(q->nodes);
}
#endif

#endif // __QUEUE_H__
