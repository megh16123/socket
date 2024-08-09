#include <stdio.h>

#define BUFFLEN 1024

void sendTobrain(FILE *f, char *buf) {
  fseek(f, 0, SEEK_END);
  fwrite(buf, BUFFLEN, 1, f);
  fseek(f, 0, SEEK_END);
}
// TODO : Display Menu
// TODO : Send COMMAND:Data to brain
// TODO : wait for brain to respond with something
// TODO : Display to the user
int main(int argc, char **argv) {
  if (argc == 3) {
    char buf[BUFFLEN];
    int old = 0;
    int new = 0;
    FILE *tobrain = fopen(argv[1], "ab"), *frombrain;
    while ((frombrain = fopen(argv[2], "rb")) == NULL)
      continue;
    while (1) {
    }
  } else {
    printf("Too few arguments were give : UI\n");
  }
  return 0;
}
