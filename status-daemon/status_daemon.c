#include "status.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile int keep_running = 1;

void handle_signal(int sig) { keep_running = 0; }

int main() {
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  while (keep_running) {
    char *status = get_status();

    pid_t pid = fork();
    if (pid == 0) {
      execlp("dwlb", "dwlb", "-status", "all", status, NULL);
      _exit(1);
    } else if (pid > 0) {
      waitpid(pid, NULL, 0);
    }

    sleep(2);
  }

  return 0;
}
