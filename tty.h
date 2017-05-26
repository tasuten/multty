#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#ifdef __APPLE__
#include <util.h>
#else
#include <pty.h>
#endif

typedef struct _tty {
  int fd;
  int pid;
} tty_t;


tty_t tty_spawn(void);
void tty_close(const tty_t self);
void tty_shell(const tty_t self, const char* shell);
