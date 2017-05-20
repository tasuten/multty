#pragma once

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tab.h"

void session_start(void);

void* input_handler(void);
void* output_handler(void);
void* signal_handler(void *tabs);
bool sigchld_handler(tab_t* tabs);
