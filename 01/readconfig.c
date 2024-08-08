#include <stdio.h>

int main(int argc, char **argv) {
  FILE *config = fopen(argv[1], "r");
  char environment[1024][1024];
  int linesRead = 0;
  int i = 0, j = 0;
  if (NULL == config) {
    printf("File didn't open \n");
  } else {
    while ((environment[i][j] = fgetc(config)) != EOF) {
      if (environment[i][j] == '\n') {
        linesRead += 1;
        environment[i][j] = '\0';
        i += 1;
        j = 0;
      } else {
        j += 1;
      }
    }
    for (i = 0; i < linesRead; i++) {
      printf("%s\n", environment[i]);
    }
  }
  return 0;
}
