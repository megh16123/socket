#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFLEN 1024

void sendmessage(int *sockfd, struct sockaddr_in *receiverAddr,
                 char *messageStr, const short int port) {
  (*receiverAddr).sin_family = AF_INET;
  (*receiverAddr).sin_port = htons(port);
  (*receiverAddr).sin_addr.s_addr = INADDR_ANY;
  printf("%s\n", messageStr);
  printf("%d\n", port);
  sendto(*sockfd, (const char *)messageStr, strlen(messageStr), 0,
         (const struct sockaddr *)receiverAddr, sizeof(*receiverAddr));
}

int main(int argc, char **argv) {

  if (argc == 2) {
    FILE *f;
    int sockfd;
    char buf[BUFFLEN];
    int old = 0;
    int dsize = 0;
    int new = 0;
    struct sockaddr_in receiverAddr;
    memset(&receiverAddr, 0, sizeof(receiverAddr));
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("Socket Failed\n");
      exit(EXIT_FAILURE);
    }
    while ((f = fopen(argv[1], "rb")) == NULL)
      continue;
    fseek(f, 0, SEEK_END);
    new = ftell(f);
    old = new;
    while (1) {
      fseek(f, 0, SEEK_END);
      new = ftell(f);
      dsize = new - old;
      if (dsize > 0) {
        fseek(f, -dsize, SEEK_END);
        dsize = dsize / BUFFLEN;
        for (int i = 0; i < dsize; i++) {
          fread(buf, sizeof(buf), 1, f);
          sendmessage(&sockfd, &receiverAddr, buf, *((short int *)buf));
          old += (i + 1) * BUFFLEN;
        }
      } else {
        continue;
      }
    }
    fclose(f);
  } else {
    printf("Too few arguments given : mouth \n");
  }

  return 0;
}
