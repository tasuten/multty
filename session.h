#pragma once

#include <pthread.h>

#include "sighandle.h"
#include "tab.h"
#include "jobq.h"
#include "session_t.h"

void session_start(void);
session_t* session_init(void);
void session_close(session_t* s);

