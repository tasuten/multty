#pragma once

#include "tab.h"

typedef struct __session {
  tab_t* tabs_head;
  tab_t* active;
  int* jobq;
} session_t;

