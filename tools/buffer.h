#ifndef __BUFFER_H
#define __BUFFER_H

typedef struct {
  char* data;
  ssize_t used, size;
} buffer_t;

void buffer_init(buffer_t*, int);
void buffer_free(buffer_t*);
ssize_t buffer_space(buffer_t*);
int buffer_full(buffer_t*);
ssize_t buffer_len(buffer_t*);
char* buffer_dup(buffer_t*);
void buffer_cpy(buffer_t*, char*);
void buffer_reset(buffer_t*, ssize_t);
void buffer_grow(buffer_t*, ssize_t);
void buffer_append(buffer_t*, char*, size_t);
void buffer_append_fmt(buffer_t*, char*, ...);

#if defined BUFFER_IMPL
void buffer_init(buffer_t* b, ssize_t s) {
  b->data = calloc(1, s * sizeof(char));
  if (!b->data) {
    fprintf(stderr, "ERROR! malloc() failed.\n");
    exit(1);
  }
  b->used = 0;
  b->size = s;
}

void buffer_free(buffer_t* b) {
  free(b->data);
  b->data = NULL;
  b->size = 0;
  b->used = 0;
}

ssize_t buffer_space(buffer_t* b) {
  return (b->size - b->used);
}

int buffer_full(buffer_t* b) {
  return (!buffer_space(b));
}

ssize_t buffer_len(buffer_t* b) {
  return b->used;
}

char* buffer_dup(buffer_t* b) {
  return strdup(b->data);
}

void buffer_cpy(buffer_t* b, char* out) {
  strcpy(out, b->data);
}

void buffer_reset(buffer_t* b, ssize_t s) {
  if (b->data)
    free(b->data);
  b->used = 0;
  b->size = s;
  b->data = calloc(1, s * sizeof(char));
}

void buffer_grow(buffer_t* b, ssize_t by) {
  ssize_t len = b->size + by;
  char* tmp = realloc(b->data, len * sizeof(char));
  b->data = tmp;
  b->size = len;
}

void buffer_append(buffer_t* b, char* str, size_t len) {
  if (buffer_full(b))
    buffer_grow(b, b->used + len - b->size);
  
  ssize_t pos = 0, copied = 0;
  for (int i = 0; i < len; ++i) {
    if (str[i] == '\0')
      break;
    
    pos = b->used + i;
    *(b->data + pos) = str[i];
    copied++;
  }
  
  b->used += copied;
  *(b->data + b->used) = '\0';
}

void buffer_append_fmt(buffer_t* b, char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  
  char* tmp = NULL;
  int len = vasprintf(&tmp, fmt, args);
  buffer_append(b, tmp, len);
  free(tmp);
  
  va_end(args);
}
#endif

#endif
