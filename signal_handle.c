#include "signal_handle.h"

const int catch[] = {SIGINT, SIGTERM, SIGCHLD, SIGTSTP};
#define n_of(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

void signal_handle_thread(session_t* session) {
  // pass through all signals from main process to the signal handler thread
  sigset_t ignore;
  sigfillset(&ignore);
  pthread_sigmask(SIG_SETMASK,  &ignore,  NULL);

  // SIGCHLD's default action is IGNore
  // Don't ignore SIGCHLD 
  struct sigaction dummy;
  dummy.sa_flags = SA_NOCLDSTOP;
  sigaction(SIGCHLD, &dummy, NULL);

  pthread_t sig;
  pthread_create(&sig, NULL, &signal_handler, (void *)session);
}

void* signal_handler(void* s) {
  pthread_detach(pthread_self());
  session_t* self = (session_t*)s;

  sigset_t look;
  sigemptyset(&look);
  for ( int i = 0; i < n_of(catch); i++ ) {
    sigaddset(&look, catch[i]);
  }

  pthread_sigmask(SIG_SETMASK, &look, NULL);

  int sig;
  packet_t pkt;
  while (1) {
    if (sigwait(&look, &sig) == 0) {
      switch(sig) {
        case SIGINT:
        case SIGTERM:
          pkt.type = QUIT_SESSION;
          jobq_send(self->jobq, pkt);
          break;
        case SIGTSTP:
          // detach
          puts("detach");
          break;
        case SIGCHLD:
          if (is_last_child(self)) {
            pkt.type = QUIT_SESSION;
            jobq_send(self->jobq, pkt);
          }
          break;
        default:
          fprintf(stderr, "Unknow signal: %s", strsignal(sig));
          exit(EXIT_FAILURE);
          break;
      }
    }
  }

  return NULL;
}

tab_t* tab_drop_by_pid(tab_t* const tabs, const pid_t pid) {
  tab_t* seeker = tabs;
  tab_t* prev = NULL;
  while (1) {
    if (seeker->tty.pid == pid) {
      // dropping
      if (prev == NULL) {
        // when the head element is matched
        return tab_close(seeker);
      } else {
        prev->next = tab_close(seeker);
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


bool is_last_child(session_t* self) {
  pid_t exited = waitpid(-1, NULL, WNOHANG);
  tab_t* next = tab_drop_by_pid(self->tabs_head, exited);
  if (next == NULL) {
    return true;
  } else {
    self->active = next;
    return false;
  }
}


