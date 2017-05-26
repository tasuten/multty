#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "tty.h"

typedef struct __tab {
  tty_t tty;
  struct __tab* next;
} tab_t;

tab_t* tab_new(tab_t* const next);
tab_t* tab_close(tab_t* const self); // return self.next

