#include "tab.h"

tab_t* tabs_init(void) {
  tty_t t = tty_spawn();
  tty_shell(t, "/bin/bash");
  tab_t* head = (tab_t *)calloc(1, sizeof(tab_t));
  head->tty = t;
  head->next = NULL;
  return head;
}

tab_t* tab_drop_by_pid(tab_t* tabs, const pid_t pid) {
  tab_t* seeker = tabs;
  tab_t* prev = NULL;
  while (1) {
    if (seeker->tty.pid == pid) {
      // dropping
      // when the head element is matched
      if (prev == NULL) {
        return seeker->next;
      } else {
        prev->next = seeker->next;
        free(seeker);
        return prev->next; // return next element
      }
    }
    if (seeker->next == NULL) {
      return NULL;
    } else {
      prev = seeker;
      seeker = seeker->next;
    }
  }
}

// add to head
tab_t* tab_new(tab_t* tabs) {
  tty_t t = tty_spawn();
  tty_shell(t, "/bin/bash");
  tab_t* new = (tab_t *)calloc(1, sizeof(tab_t));
  new->tty = t;
  new->next = tabs;

  return new;
}
