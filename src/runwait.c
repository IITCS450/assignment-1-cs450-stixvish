#define _POSIX_C_SOURCE 199309L
#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void usage(const char *a) {
  fprintf(stderr,"Usage: %s <cmd> [args]\n",a);
  exit(1);
}

static double d(struct timespec a, struct timespec b) {
  return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;
}

int main(int c,char**v) {
  if (c < 2) usage(v[0]);

  struct timespec start, end;
  int status;

  // start the timer
  clock_gettime(CLOCK_MONOTONIC, &start);
  // fork t he process
  pid_t pid = fork();
  
  if (pid < 0) {
    perror("fork failed");
    return 1;
  } else if (pid == 0) {
    // child process
    execvp(v[1], &v[1]);
    perror("execvp failed");
    exit(1);
  } else {
    // parent process
    waitpid(pid, &status, 0);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("pid=%d elapsed=%.3f exit=%d\n", pid, d(start, end), WEXITSTATUS(status));
  }

  return 0;
}
