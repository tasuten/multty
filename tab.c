#include "tab.h"

tab_elem_t* tab_init(void) {
  tty_t t = tty_spawn();
  tab_elem_t* head = (tab_elem_t *)calloc(1, sizeof(tab_elem_t));
  head->tty = t;
  head->next = NULL;
  return head;
}

tab_elem_t tab_nth(tab_elem_t* self, int n) {
  if ( n == 0 ) {
    return *self;
  } else {
    exit(1);
  }
}
