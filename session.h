#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tab.h"
#include "jobq.h"

typedef struct __session {
  tab_t* tabs_head;
  tab_t* active;
  pthread_mutex_t* active_mutex;
} session_t;

void session_start(void);

void* stdin_handler(void *jobq);
void* tty_handler(void *jobq);
void* consume_queue(void *jobq);
void* signal_handler(void* _);
bool sigchld_handler(void);
