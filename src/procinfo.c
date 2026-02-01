#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static void usage(const char *a) {
  fprintf(stderr, "Usage: %s <pid>\n", a);
  exit(1);
}

static int isnum(const char *s) {
  for (; *s; s++) if (!isdigit(*s)) return 0;
  return 1;
}

int main(int c, char **v) {
  if (c != 2 || !isnum(v[1])) usage(v[0]);

  // variables for file paths and reading lines
  char path[256];
  char line[256];
  FILE *f;

  // find info from the status file'
  snprintf(path, sizeof(path), "/proc/%s/status", v[1]);
  f = fopen(path, "r");
  if (!f) {
    perror("error opening status file");
    return 1;
  }
  printf("PID: %s\n", v[1]);
  while (fgets(line, sizeof(line), f)) {
    if (strncmp(line, "State:", 6) == 0) {
      char state;
      sscanf(line, "State:\t%c", &state);
      printf("State: %c\n", state);
    } else if (strncmp(line, "PPid:", 5) == 0) {
      int ppid;
      sscanf(line, "PPid:\t%d", &ppid);
      printf("PPid: %d\n", ppid);
    } else if (strncmp(line, "VmRSS:", 6) == 0) {
      int rss;
      sscanf(line, "VmRSS:\t%d", &rss);
      printf("VmRSS: %d\n", rss);
    }
  }
  fclose(f);

  // find info from the cmdline file
  snprintf(path, sizeof(path), "/proc/%s/cmdline", v[1]);
  f = fopen(path, "r");
  if (!f) {
    perror("error opening cmdline file");
    return 1;
  }
  int ch;
  printf("Cmd: ");
  while ((ch = fgetc(f)) != EOF) {
    if (ch == '\0') {
      int next = fgetc(f);
      if (next != EOF && next != '\0') {
        printf(" ");
        ungetc(next, f);
      }
    } else {
      printf("%c", ch);
    }
  }
  printf("\n");
  fclose(f);

  // find info from the stat file
  snprintf(path, sizeof(path), "/proc/%s/stat", v[1]);
  f = fopen(path, "r");
  if (!f) {
    perror("error opening stat file");
    return 1;
  }
  unsigned long utime, stime;
  if (fscanf(f, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime) == 2) {
    printf("CPU: %lu %lu\n", utime, stime);
  }
  fclose(f);

  return 0;
}