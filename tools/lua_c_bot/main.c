//
//  main.c
//  test
//
//  Created by Rory B. Bellows on 14/11/2017.
//  Copyright © 2017 Rory B. Bellows. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define MSG_BUF 512
static int sock = 0;

typedef struct {
  char* data;
  ssize_t used, size;
} buffer_t;

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

void lua_fail(lua_State *L, char *msg){
  fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n", msg, lua_tostring(L, -1));
  exit(1);
}
#define LUA(x) if (x) lua_fail(L, #x)
#define LUA_LOAD(x) LUA(luaL_loadfile(L, x))
#define LUA_PCALL(x, y, z) LUA(lua_pcall(L, x, y, z))

#define THROW(msg) \
lua_getglobal(L, "on_error"); \
lua_pushstring(L, msg); \
LUA_PCALL(1, 0, 0);

static int send_raw(lua_State* L) {
  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 arguments");

  const char* msg = lua_tostring(L, 1);
  size_t msg_len = strlen(msg) + 3;
  char buf[msg_len];
  sprintf(buf, "%s\r\n", msg);
  write(sock, buf, msg_len);

  return 0;
}

int main(int argc, const char* argv[]) {
  if (argc < 4) {
    fprintf(stderr, "ERROR: %s [script.lua] [host] [port]\n", argv[0]);
    return 1;
  }

  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  LUA_LOAD(argv[1]);
  lua_pushcfunction(L, send_raw);
  lua_setglobal(L, "send");
  LUA_PCALL(0, 0, 0);

  int rv = 0;
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if ((rv = getaddrinfo(argv[2], argv[3], &hints, &res)) != 0) {
    THROW(gai_strerror(rv));
    return 1;
  }

  if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
    THROW("Failed to create socket");
    return 1;
  }

  if (connect(sock, res->ai_addr, res->ai_addrlen) == -1) {
    THROW("Failed to connect");
    return 1;
  }

  lua_getglobal(L, "on_connect");
  LUA_PCALL(0, 0, 0);

  buffer_t buf;
  buffer_init(&buf, MSG_BUF);

  char sbuf[MSG_BUF + 1];
  ssize_t rl = 0;
  while ((rl = read(sock, sbuf, MSG_BUF))) {
    if (rl < 1) {
      THROW("Failed to read from socket");
      break;
    }

    if (!strstr(sbuf, "\r\n"))
      buffer_append(&buf, sbuf, rl);
    else {
      char* token = strtok(sbuf, "\r\n");
      while (token) {
        lua_getglobal(L, "on_msg");
        if (buf.used) {
          buffer_append(&buf, token, strlen(token));
          lua_pushstring(L, buf.data);
          buffer_reset(&buf, MSG_BUF);
        } else
          lua_pushstring(L, token);
        LUA_PCALL(1, 0, 0);
        token = strtok(NULL, "\r\n");
      }
    }
    memset(sbuf, 0, MSG_BUF + 1);
  }

  lua_getglobal(L, "on_exit");
  LUA_PCALL(0, 0, 0);

  lua_close(L);
  buffer_free(&buf);
  shutdown(sock, SHUT_RDWR);
  close(sock);

  return 0;
}
