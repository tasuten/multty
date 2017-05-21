#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "tty.h"

typedef struct __tab {
  tty_t tty;
  struct __tab* next;
} tab_t;

tab_t* tabs_list(void);
tab_t* tab_drop_by_pid(tab_t* const tabs, const int n);
tab_t* tab_new(tab_t* const tabs);
