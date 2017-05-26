#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>

#include "jobq.h"
#include "signal_handle.h"
#include "session_t.h"



void session_start(void);

void* stdin_handler(void *self);
void* tty_handler(void *self);
void* consume_queue(void *self);
