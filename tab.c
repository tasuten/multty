#include "tab.h"

tab_t* tab_new(tab_t* const next) {
  tty_t t = tty_spawn();
  tty_shell(t, "/bin/bash");
  tab_t* head = (tab_t *)calloc(1, sizeof(tab_t));
  head->tty = t;
  head->next = next;
  return head;
}

tab_t* tab_close(tab_t* const self) {
  tab_t* next = self->next;
  self->next = NULL;
  tty_close(self->tty);
  free(self);
  return next;
}

