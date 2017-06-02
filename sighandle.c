#include "sighandle.h"

const static int sigs[] = {SIGINT, SIGTERM, SIGTSTP, SIGCHLD, SIGWINCH};
#define n_of(arr) ((int)(sizeof(arr)/sizeof((arr)[0])))

void init_sighandle() {
  // main thread passes all signals to sighandle thread
  sigset_t pass;
  sigfillset(&pass);
  pthread_sigmask(SIG_SETMASK, &pass, NULL);
}

void* sighandle_loop(void* s) {
  pthread_detach(pthread_self());
  session_t* self = (session_t*)s;

  sigset_t receive;
  sigemptyset(&receive);

  for (int i = 0; i < n_of(sigs); i++) {
    sigaddset(&receive, sigs[i]);
  }

  // Don't ignore SIGCHLD, SIGWINCH
  signal(SIGCHLD, _nopmock);
  signal(SIGWINCH, _nopmock);

  int sig;
  packet_t pkt;
  while(1){
    if (sigwait(&receive, &sig) == 0) {
      switch(sig) {
        case SIGINT:
        case SIGTERM:
          pkt.type = QUIT_SESSION;
          jobq_send(self->jobq, pkt);
          break;
        case SIGTSTP:
          pkt.type = DETACH_SESSION;
          jobq_send(self->jobq, pkt);
          break;
        case SIGCHLD:
          pkt.type = CHILD_DIED;
          pkt.child = who_died();
          jobq_send(self->jobq, pkt);
          break;
        case SIGWINCH:
          resize_window();
          break;
        default:
          fprintf(stderr, "Unknown signal: %s\n", strsignal(sig));
          exit(EXIT_FAILURE);
          break;
      }
    }
  }
  return NULL;
}

void _nopmock(int _signum) {
  _signum = 0;
}

void resize_window(void) {
  struct winsize ws;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
  ioctl(STDIN_FILENO, TIOCSWINSZ, &ws);
}

pid_t who_died(void) {
  pid_t died;
  died = waitpid(-1, NULL, WNOHANG);
  if (died <= 0) { // including `kill -CHLD multty`
    fprintf(stderr, "Something wrong on detect the dead pid: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return died;
}
