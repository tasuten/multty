#include "session.h"

#define BUFLEN 256

#define N_SIGNALS 4
const int block_signals[N_SIGNALS] = {SIGINT, SIGTERM, SIGCHLD, SIGTSTP};

tab_t* active = NULL;

void session_start(void) {
  tab_t* tabs = tabs_init();
  active = tabs;


  // pass through signals to the signal_handler thread
  sigset_t ignore;
  sigfillset(&ignore);
  pthread_sigmask(SIG_SETMASK,  &ignore,  NULL);

  // SIGCHLD's default action is IGNore
  // So, it doesn't ignore this
  struct sigaction dummy;
  dummy.sa_flags = SA_NOCLDSTOP;
  sigaction(SIGCHLD, &dummy, NULL);

  pthread_t sig;
  pthread_create(&sig, NULL, &signal_handler, tabs);

  pthread_t in, out;
  pthread_create(&in, NULL, &input_handler, NULL);
  pthread_create(&out, NULL, &output_handler, NULL);

  pthread_join(sig, NULL);
}

void* input_handler(void) {
  pthread_detach(pthread_self());
  int fd = active->tty.master_fd;
  ssize_t nread = 0;
  char buf[BUFLEN];
  while(1) {
    nread = read(STDIN_FILENO,  buf,  BUFLEN);

    if (nread < 0 || nread == 0) break;

    if (write(fd,  buf,  (size_t)nread) != nread) break;
  }

  // puts("deadie");
  return NULL;
}


void* output_handler(void) {
  pthread_detach(pthread_self());
  int fd = active->tty.master_fd;
  ssize_t nread = 0;
  char buf[BUFLEN];
  while(1) {
    if ((nread = read(fd,  buf,  BUFLEN)) <= 0) break;

    if (write(STDOUT_FILENO,  buf,  (size_t)nread) != nread) break;
  }

  // puts("deadoo");
  return NULL;
}

void* signal_handler(void *tabs) {

  sigset_t looking;
  sigemptyset(&looking);
  for ( int i = 0; i < N_SIGNALS; i++ ) {
    sigaddset(&looking, block_signals[i]);
  }

  pthread_sigmask(SIG_SETMASK, &looking, NULL);

  bool fin = false;

  int sig;
  while (!fin) {
    if (sigwait(&looking, &sig) == 0) {
      switch(sig) {
        case SIGINT:
        case SIGTERM:
          fin = true;
          break;
        case SIGTSTP:
          // detach
          puts("detach");
          break;
        case SIGCHLD:
          fin = sigchld_handler((tab_t *)tabs);
          break;
        default:
          fprintf(stderr, "Unknow signal: %s", strsignal(sig));
          exit(EXIT_FAILURE);
          break;
      }
    }
  }

  // puts("deadsigi");
  return NULL;
}

bool sigchld_handler(tab_t* tabs) {
  pid_t exited = waitpid(-1, NULL, WNOHANG);
  tab_t* next = tab_drop_by_pid(tabs, exited);
  if (next == NULL) {
    return true;
  } else {
    active = next;
    return false;
  }
}

