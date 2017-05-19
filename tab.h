#pragma once

#include <stdlib.h>
#include "tty.h"

typedef struct _tty_entry {
  tty_t tty;
  tty_t* next;
} tab_elem_t;

tab_elem_t* tab_init(void);
tab_elem_t tab_nth(tab_elem_t* self, int n);
