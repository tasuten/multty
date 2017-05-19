#include "session.h"

#define BUFLEN 256

void session_start(void) {
  tab_elem_t* tabs = tab_init();

  // search active tab
  // and
  pthread_t in, out;
  pthread_create(&in, NULL, &input_handler, tabs);
  pthread_create(&out, NULL, &output_handler, tabs);
  for(;;){}
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
