#ifndef __DEBUG_h
#define __DEBUG_h

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define LOG(MSG, ...) fprintf(stderr, "[DEBUG] from %s in %s at %d -- " MSG "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define GET_ERRNO() (errno == 0 ? "None" : strerror(errno))

#define LOG_ERR(MSG, ...) fprintf(stderr, "[ERROR] from %s in %s at %d (errno: %s) " MSG "\n", __FILE__, __FUNCTION__, __LINE__, GET_ERRNO(), ##__VA_ARGS__)

#endif
