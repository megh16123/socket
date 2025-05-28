#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

FILE *f;
static int BUFLEN;
void sendTobrain(char *fname,unsigned char *buf) {
unsigned char ch;	
	f = fopen(fname,"rb+");
	fseek(f,0,SEEK_SET);
	while((ch = fgetc(f))!='1'){
		fclose(f);
		f = fopen(fname,"rb+");
		fseek(f,0,SEEK_SET);
	}
	//TODO: if domain of ch expands insert if ch == 1  here

	ch = '0';
	fseek(f,1,SEEK_SET);
	fwrite(buf,sizeof(unsigned char),BUFLEN,f);
	fseek(f,0,SEEK_SET);
	fputc(ch,f);
	fclose(f);
}

int main(int argc, char **argv) {
  if (argc == 4) {
    int LISTENPORT = atoi(argv[1]);
    BUFLEN = atoi(argv[2]);
    int sockfd, n;
    socklen_t len;
    unsigned char buffer[BUFLEN];
    struct sockaddr_in receiverAddr, senderAddr;
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
    memset(buffer, 0, BUFLEN);
      n = recvfrom(sockfd, buffer, BUFLEN, MSG_WAITALL, (struct sockaddr *)&senderAddr, &len);
      sendTobrain(argv[3], buffer);
    }
    fclose(f);
  } else {
    printf("Too few arguments were given : ear\n");
  }

  return 0;
}
