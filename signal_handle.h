#pragma once

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "session_t.h"
#include "tab.h"
#include "jobq.h"

void signal_handle_thread(session_t* session);
void* signal_handler(void* s);
bool is_last_child(session_t* self);
