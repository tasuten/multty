#pragma once

#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <pthread.h>

#include "session_t.h"

void init_sighandle(void);
void* sighandle_loop(void* self);
