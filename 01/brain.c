#include <stdio.h>

#define BUFFLEN 1024

void sendToFile(FILE *f, char *buf) {
  fseek(f, 0, SEEK_END);
  fwrite(buf, BUFFLEN, 1, f);
  fseek(f, 0, SEEK_END);
}
// TODO : Initiate ID:port table
// TODO : Setup the Environment
// TODO : Write command:data kind of interface between UI and Brain
// TODO : Broadcast to other machines
// TODO : Processing Language needs to be decided
int main(int argc, char **argv) {
  if (argc == 2) {
    FILE *config = fopen(argv[1], "r");
    char environment[1024][1024], messages[1024][1024];
    int linesRead = 0;
    int i = 0, j = 0, mnum = 0, messages_came = 0, no_of_machines = 0;
    char buf[BUFFLEN];
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
      no_of_machines = linesRead - 6;
      FILE *be, *bm, *uio, *uii;
      bm = fopen(environment[3], "ab");
      uii = fopen(environment[5], "ab");
      while (((be = fopen(environment[2], "rb")) == NULL) ||
             ((uio = fopen(environment[4], "rb")) == NULL))
        continue;
      fseek(be, 0, SEEK_END);
      fseek(uio, 0, SEEK_END);
      int unew = ftell(uio), uold = ftell(uio), enew = ftell(be), eold = ftell(be), dsize = 0;
      while (1) {
        // read from ear
        fseek(be, 0, SEEK_END);
        enew = ftell(be);
        dsize = enew - eold;
        if (dsize > 0) {
          fseek(be, -dsize, SEEK_END);
          dsize = dsize / BUFFLEN;
          for (int i = 0; i < dsize; i++) {
            fread(buf, sizeof(buf), 1, be);
            sprintf(messages[messages_came], "%d:%s", *((short int *)buf),
                    ((char *)(((short int *)buf) + 1)));
            messages_came += 1;
            printf("%d\n", *((short int *)buf));
            printf("%s\n", ((char *)(((short int *)buf) + 1)));
            eold += (i + 1) * BUFFLEN;
          }
        }
        // read from ui
        fseek(uio, 0, SEEK_END);
        unew = ftell(uio);
        dsize = unew - uold;
        if (dsize > 0) {
          fseek(uio, -dsize, SEEK_END);
          dsize = dsize / BUFFLEN;
          for (i = 0; i < dsize; i++) {
            fread(buf, sizeof(buf), 1, uio);
            fseek(uio, BUFFLEN, SEEK_CUR);
            switch (buf[0]) {
            case '1':
              break;
            case '2':
              *((int *)buf) = messages_came;
              sendToFile(uii, buf);
              for (j = 0; j < messages_came; j++) {
                sendToFile(uii, messages[j]);
              }
              break;
            case '3':
              *((int *)buf) = no_of_machines;
              sendToFile(uii, buf);
              for (j = 0; j < no_of_machines; j++) {
                sendToFile(uii, environment[j + 6]);
              }
              break;
            }
            // process buffer -> may send to mouth or ui
            uold += (i + 1) * BUFFLEN;
          }
        }
      }
    }

  } else {
    printf("Too few arguments : brain\n");
  }
  return 0;
}
