#include <stdio.h>

#define BUFFLEN 1024

int main(int argc, char **argv) {
  if (argc == 2) {
    FILE *config = fopen(argv[1], "r");
    char environment[1024][1024];
    int linesRead = 0;
    int i = 0, j = 0;
    char buf[BUFFLEN];
    int unew = 0, uold = 0, enew = 0, eold = 0;
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
      FILE *be, *bm, *uio, *uii;
      bm = fopen(environment[3], "ab");
      uii = fopen(environment[5], "ab");
      while (((be = fopen(environment[2], "rb")) == NULL) ||
             ((uio = fopen(environment[4], "rb")) == NULL))
        continue;
      while (1) {
        // read from ear
        // process buffer -> may send to ui or mouth
        // read from ui
        // process buffer -> may send to mouth or ui
      }
    }

  } else {
    printf("Too few arguments : brain\n");
  }
  return 0;
}
