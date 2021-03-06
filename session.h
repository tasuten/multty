#pragma once

#include <stdbool.h>

#include <poll.h>

#include <pthread.h>

#include "sighandle.h"
#include "tab.h"
#include "jobq.h"
#include "session_t.h"

void session_start(void);
session_t* session_init(void);
void session_close(session_t* s);
void session_task_loop(session_t* self);

bool handle_message_packet(packet_t pkt);
bool handle_child_died_packet(packet_t pkt, session_t* self);
void* stdin_loop(void* self);
void* ttyout_loop(void* self);
