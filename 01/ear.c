#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFLEN 1024

void sendTobrain(FILE *f, char *buf) {
  fseek(f, 0, SEEK_END);
  fwrite(buf, BUFLEN, 1, f);
  fseek(f, 0, SEEK_END);
}

int main(int argc, char **argv) {
  if (argc == 3) {
    int LISTENPORT = atoi(argv[1]);
    int sockfd, n;
    socklen_t len;
    char buffer[BUFLEN];
    struct sockaddr_in receiverAddr, senderAddr;

    FILE *f = fopen(argv[2], "ab");
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket system call failed");
      exit(EXIT_FAILURE);
    }

    memset(&receiverAddr, 0, sizeof(receiverAddr));
    memset(&senderAddr, 0, sizeof(senderAddr));

    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(LISTENPORT);
    receiverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (const struct sockaddr *)&receiverAddr,
             sizeof(receiverAddr)) < 0) {
      perror("bind syscall failed");
      exit(EXIT_FAILURE);
    }
    len = sizeof(senderAddr);
    while (1) {
      n = recvfrom(sockfd, buffer, BUFLEN - (sizeof(short int)), MSG_WAITALL,
                   (struct sockaddr *)&senderAddr, &len);
      buffer[n] = '\0';
      sendTobrain(f, buffer);
    }
    fclose(f);
  } else {
    printf("Too few arguments were given : ear\n");
  }

  return 0;
}
