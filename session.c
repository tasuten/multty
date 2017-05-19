#include "session.h"

#define BUFLEN 256

int block_signals[] = { SIGINT, SIGTERM, SIGCHLD, SIGTSTP};
#define N_SIGNALS 4

void session_start(void) {
  tab_elem_t* tabs = tab_init();

  // search active tab
  tab_elem_t* tab = tabs; // stub
  // and

  sigset_t ignore;
  sigfillset(&ignore);
  struct sigaction dummy;
  dummy.sa_flags = SA_NOCLDSTOP;
  sigaction(SIGCHLD, &dummy, NULL);
  pthread_sigmask(SIG_SETMASK,  &ignore,  NULL);

  pthread_t in, out;
  pthread_t sig;
  pthread_create(&in, NULL, &input_handler, tab);
  pthread_create(&out, NULL, &output_handler, tab);
  pthread_create(&sig, NULL, &signal_handler, tabs);

  pthread_join(sig, NULL);
}

void* input_handler(void *tab) {
  pthread_detach(pthread_self());
  int fd = ((tab_elem_t *)tab)->tty.master_fd;
  int nread = -1;
  char buf[BUFLEN];
  while(1) {
    nread = read(STDIN_FILENO,  buf,  BUFLEN);

    if (nread < 0 || nread == 0) break;

    if (write(fd,  buf,  nread) != nread) break;
  }

  puts("deadie");
  return NULL;
}


void* output_handler(void *tab) {
  pthread_detach(pthread_self());
  int fd = ((tab_elem_t *)tab)->tty.master_fd;
  int nread = -1;
  char buf[BUFLEN];
  while(1) {
    if ((nread = read(fd,  buf,  BUFLEN)) <= 0) break;

    if (write(STDOUT_FILENO,  buf,  nread) != nread) break;
  }

  puts("deadoo");
  return NULL;
}

void* signal_handler(void *tabs) {

  sigset_t set;
  sigemptyset(&set);
  for ( int i = 0; i < N_SIGNALS; i++ ) {
    sigaddset(&set, block_signals[i]);
  }

  pthread_sigmask(SIG_SETMASK, &set, NULL);

  bool fin = false;

  int sig;
  while (!fin) {
    if (sigwait(&set, &sig) == 0) {
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
          puts("sigchld");
          break;
        default:
          fprintf(stderr, "Unknow signal: %s", strsignal(sig));
          exit(EXIT_FAILURE);
          break;
      }
    }
  }

  puts("deadsigi");
  return NULL;
}
