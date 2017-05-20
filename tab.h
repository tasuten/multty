#pragma once

#include <stdlib.h>
#include "tty.h"

typedef struct _tty_entry {
  tty_t tty;
  tty_t* next;
} tab_t;

tab_t* tab_init(void);
tab_t tab_nth(tab_t* tabs, int n);
