#include <stdio.h>

#define BUFFLEN 1024

int dsize = 0;
void sendTobrain(FILE *f, char *buf) {
  fseek(f, 0, SEEK_END);
  fwrite(buf, BUFFLEN, 1, f);
  fseek(f, 0, SEEK_END);
}
void getFromBrain(FILE *f, int *old, int *new, char *buf) {
  char flg = 0;
  while (flg == 0) {
    // read from ear
    fseek(f, 0, SEEK_END);
    *new = ftell(f);
    dsize = *new - *old;
    // printf("DSIZE : %d\n", dsize);
    if (dsize > 0) {
      flg = 1;
    }
  }
  fseek(f, -dsize, SEEK_END);
  dsize = dsize / BUFFLEN;
  fread(buf, BUFFLEN, 1, f);
  *old += BUFFLEN;
  fseek(f, BUFFLEN, SEEK_CUR);
}
// TODO : Display Menu
// TODO : Send COMMAND:Data to brain
// TODO : wait for brain to respond with something
// TODO : Display to the user
int main(int argc, char **argv) {
  if (argc == 3) {
    char buf[BUFFLEN];
    int old = 0;
    int new = 0, mnum = 0;
    char choice;
    char *message;
    FILE *tobrain = fopen(argv[1], "ab"), *frombrain;
    printf("UI L: %s\n",argv[1]);
    printf("fileeee :  %s\n", argv[2]);
    while ((frombrain = fopen(argv[2], "rb")) == NULL)
      continue;
    fseek(tobrain, 0, SEEK_END);
    new = ftell(tobrain);
    old = new;
    while (1) {
      printf("1. Send message\t2. Read Messages\t3. List Systems\n");
      choice = getchar();
      getchar();
      switch (choice) {
      case '1':
        buf[0] = choice;
        printf("Who to send : ");
        mnum = 1;
        buf[mnum] = '\n';
        mnum += 1;
        while ((buf[mnum] = getchar()) != '\n') {
          mnum += 1;
        }
        buf[mnum] = '\n';
        mnum += 1;
        printf("What to send : ");
        while ((buf[mnum] = getchar()) != '\n') {
          mnum += 1;
        }

        buf[mnum] = '\n';
        mnum += 1;
        printf("%s", buf);
        sendTobrain(tobrain, buf);
        getFromBrain(frombrain, &old, &new, buf);
        break;
      case '2':
        *buf = choice;
        sendTobrain(tobrain, buf);
        getFromBrain(frombrain, &old, &new, buf);
        mnum = *((int *)buf);
        while (mnum > 0) {
          getFromBrain(frombrain, &old, &new, buf);
          printf("%s\n", buf);
          mnum -= 1;
        }
        break;
      case '3':
        *buf = choice;
        sendTobrain(tobrain, buf);
        getFromBrain(frombrain, &old, &new, buf);
        mnum = *((int *)buf);
        while (mnum > 0) {
          getFromBrain(frombrain, &old, &new, buf);
          printf("%s\n", buf);
          mnum -= 1;
        }
        break;
      default:
        printf("Please choose a correct option\n");
      }
    }
  } else {
    printf("Too few arguments were give : UI\n");
  }
  return 0;
}
