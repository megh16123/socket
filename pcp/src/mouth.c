#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static int BUFLEN;

#define clm(x) (memset(x, 0, sizeof(x)))
void sendmessage(int *sockfd, struct sockaddr_in *receiverAddr,unsigned char *messageStr, const short int port) {
  (*receiverAddr).sin_family = AF_INET;
  (*receiverAddr).sin_port = htons(port);
  (*receiverAddr).sin_addr.s_addr = INADDR_ANY;
  sendto(*sockfd, (const unsigned char *)messageStr, BUFLEN-sizeof(short int), 0, (const struct sockaddr *)receiverAddr, sizeof(struct sockaddr_in));
}


int main(int argc, char **argv) {

  if (argc == 3) {
    BUFLEN = atoi(argv[2]) + sizeof(short int);
    FILE *f;
    int sockfd;
    unsigned char buf[BUFLEN];
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
      if (dsize > 0 && 0 == (dsize % BUFLEN)) {
      	if((dsize/BUFLEN) > 1) dsize = BUFLEN;
          fseek(f, old, SEEK_SET);
          dsize = dsize / BUFLEN;
          clm(buf);
          fread(buf, BUFLEN, 1, f);
          sendmessage(&sockfd, &receiverAddr, buf + sizeof(short int), *((short int *)buf));
          old +=  BUFLEN;
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
