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

void session_start(void);

void* stdin_handler(void *jobq);
void* tty_handler(void *jobq);
void* consume_queue(void *jobq);
void* signal_handler(void *tabs);
bool sigchld_handler(tab_t* tabs);
