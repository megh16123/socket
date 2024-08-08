#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc == 2) {
    FILE *config = fopen(argv[1], "r");
    char *programs[4] = {"brain", "ear", "mouth", "ui"};
    char environment[1024][1024];
    int linesRead = 0;
    int pid[3], pi = 0, re;
    int i = 0, j = 0;
    char *bargs[3] = {programs[0], argv[1], NULL}, *eargs[4], *margs[3],
         *uargs[4];
    if (NULL == config) {
      printf("File didn't open \n");
    } else {
      while ((environment[i][j] = fgetc(config)) != EOF && linesRead < 6) {
        if (environment[i][j] == '\n') {
          linesRead += 1;
          environment[i][j] = '\0';
          i += 1;
          j = 0;
        } else {
          j += 1;
        }
      }
    }
    eargs[0] = programs[1];
    eargs[1] = environment[1];
    eargs[2] = environment[2];
    eargs[3] = NULL;
    margs[0] = programs[2];
    margs[1] = environment[3];
    margs[2] = NULL;
    uargs[0] = programs[3];
    uargs[1] = environment[4];
    uargs[2] = environment[6];
    uargs[3] = NULL;
    ///////////////////////////////////////////////////////////////////////////

    pid[pi] = fork();
    if (pid[pi] > 0) {
      re = execv(programs[pi], (char **)bargs);
      if (re == -1) {
        printf("Error Running\n");
      }
    } else if (pid[pi] == 0) {
      pi += 1;
      pid[pi] = fork();
      if (pid[pi] > 0) {
        re = execv(programs[pi], (char **)eargs);
        if (re == -1) {
          printf("Error Running 2\n");
        }
      } else if (pid[pi] == 0) {
        pi += 1;
        pid[pi] = fork();
        if (pid[pi] > 0) {
          re = execv(programs[pi], (char **)margs);
        } else if (pid[pi] == 0) {
          pi += 1;
          re = execv(programs[pi], (char **)uargs);
        } else {
          printf("Error in forking 3\n");
        }
      } else {
        printf("Error in forking 2\n");
      }
    } else {
      printf("Error in forking\n");
    }
  } else {
    printf("Too few arguments given : launcher\n");
  }
  return 0;
}
