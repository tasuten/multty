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

  int* queue = jobq_open();
  pthread_t in;
  pthread_create(&in, NULL, &stdin_handler, (void *)queue);
  pthread_t tty;
  pthread_create(&tty, NULL, &tty_handler, (void *)queue);
  pthread_t consumer;
  pthread_create(&consumer, NULL, &consume_queue, (void *)queue);


  pthread_join(sig, NULL);
  jobq_close(queue);
}

void* stdin_handler(void* jobq) {
  pthread_detach(pthread_self());
  int* q = (int *)jobq;
  ssize_t nread = 0;
  char buf[PAYLOAD_LEN];
  packet_t pkt;
  while(1) {
    nread = read(STDIN_FILENO,  buf,  PAYLOAD_LEN);

    if (nread < 0 || nread == 0) break;

    strncpy(pkt.payload, buf, nread);
    pkt.type = MESSAGE;
    pkt.dest = active->tty.master_fd;
    pkt.len = nread;

    jobq_send(q, pkt);
  }

  return NULL;
}

void* tty_handler(void* jobq) {
  pthread_detach(pthread_self());
  int* q = (int *)jobq;
  ssize_t nread = 0;
  char buf[PAYLOAD_LEN];
  packet_t pkt;
  while(1) {
    nread = read(active->tty.master_fd, buf, PAYLOAD_LEN);

    if (nread < 0 || nread == 0) break;

    strncpy(pkt.payload, buf, nread);
    pkt.type = MESSAGE;
    pkt.dest = STDOUT_FILENO;
    pkt.len = nread;
    jobq_send(q, pkt);

  }

  return NULL;
}

void* consume_queue(void *jobq) {
  pthread_detach(pthread_self());
  int* q = (int *)jobq;
  packet_t pkt;
  while(1) {
    pkt = jobq_recv(q);

    if (pkt.type == FIN) break;

    if (pkt.type == MESSAGE) {
      write(pkt.dest,  pkt.payload, pkt.len);
    }
  }

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

