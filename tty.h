#pragma once

#if defined(__APPLE__)
#include <util.h>
#else
#include <pty.h>
#endif

typedef struct _tty {
  int master_fd; 
  int pid;
} tty_t;


tty_t tty_spawn(void);