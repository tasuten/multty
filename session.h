#pragma once

#include <pthread.h>

#include "tab.h"

void session_start(void);

void* input_handler(void *tab);
void* output_handler(void *tab);
