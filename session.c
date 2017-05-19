#include "session.h"

void session_start(void) {
  tab_elem_t* tabs = tab_init();
  printf("%d\n", tabs->tty.pid);
}
