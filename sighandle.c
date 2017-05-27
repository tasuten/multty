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

  // ToDo: Don't ignore SIGCHLD, SIGWINCH

  int sig;
  while(1){
    if (sigwait(&receive, &sig) == 0) {
      // switch(sig){
          printf("%s\n", strsignal(sig));
      // }
    }
  }
  return NULL;
}

