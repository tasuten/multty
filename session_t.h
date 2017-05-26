#pragma once

#include "tab.h"

typedef struct __session {
  tab_t* tabs_head;
  tab_t* active;
  pthread_mutex_t* active_mutex;
  int* jobq;
} session_t;

