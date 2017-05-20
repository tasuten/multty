#include "tty.h"

tty_t tty_spawn(void) {
  tty_t new = {
    .master_fd = -1,
    .pid = -1,
  };

  new.pid = forkpty(&new.master_fd, NULL, NULL, NULL);
  if ( new.pid < 0 ) {
    fprintf(stderr, "forkpty() failed with error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return new;
}

void tty_shell(const tty_t self, const char* shell) {
  if (self.pid == 0) { // child
    // exec login shell
    char* p = strrchr(shell, '/');p++; // basename
    char *login_shell;
    asprintf(&login_shell, "-%s", p); // asprintf requires free(), but it'll exec
    if ( execl("/bin/sh", login_shell, NULL) == -1) {
      fprintf(stderr, "exec() failed with error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  } else if (self.pid > 0){ // parent
    // set terminal to raw
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    cfmakeraw(&raw);
    // polling
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  }
}
