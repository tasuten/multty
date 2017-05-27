#pragma once

#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "session_t.h"
#include "jobq.h"

void init_sighandle(void);
void* sighandle_loop(void* self);
void _nopmock(int _signum);
void resize_window(void);
pid_t who_died(void);
