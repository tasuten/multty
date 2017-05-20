#include "tab.h"

tab_t* tab_init(void) {
  tty_t t = tty_spawn();
  tty_shell(t, "/bin/bash");
  tab_t* head = (tab_t *)calloc(1, sizeof(tab_t));
  head->tty = t;
  head->next = NULL;
  return head;
}

// tab_t tab_by_id(tab_t* tabs, pid_t pid) {
// }
