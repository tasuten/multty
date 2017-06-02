#pragma once

#include "tab.h"
#include "jobq.h"

typedef struct __session {
  tab_t* tabs_head;
  tab_t* active;
  mpsc_t* jobq;
  pthread_rwlock_t* rwlock;
} session_t;

