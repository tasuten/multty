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

void* input_handler(void *tab);
void* output_handler(void *tab);
void* signal_handler(void *tabs);
void sigchld_handler(void);
