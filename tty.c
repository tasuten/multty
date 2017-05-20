#include "tty.h"

tty_t tty_spawn(void) {
  tty_t result = {
    .master_fd = -1,
    .pid = -1,
  };

  result.pid = forkpty(&result.master_fd, NULL, NULL, NULL);
  if ( result.pid == -1 ) {
    fprintf(stderr, "forkpty() died with error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return result;
}

void tty_shell(const tty_t self, const char* sh) {
  if (self.pid == 0) {
    // child
    char* p = strrchr(sh, '/');p++; // basename
    char *login_shell = NULL;
    asprintf(&login_shell, "-%s", p); // requires free(), but it'll exec
    if ( execl("/bin/sh", login_shell, NULL) == -1) {
      fprintf(stderr, "exec() died with error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  } else {
    // parent
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    cfmakeraw(&raw);
    // polling
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  }

}
