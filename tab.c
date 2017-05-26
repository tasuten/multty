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

tab_t* tab_drop_by_pid(tab_t* const tabs, const pid_t pid) {
  tab_t* seeker = tabs;
  tab_t* prev = NULL;
  while (1) {
    if (seeker->tty.pid == pid) {
      // dropping
      if (prev == NULL) {
      // when the head element is matched
        tab_t* ret = seeker->next;
        free(seeker);
        return ret;
      } else {
        prev->next = seeker->next;
        free(seeker);
        return prev->next; // return next element
      }
    }
    // no match found 
    if (seeker->next == NULL) {
      return NULL;
    } else {
      // next element
      prev = seeker;
      seeker = seeker->next;
    }
  }
}

