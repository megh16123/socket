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
void sendmessage(int *sockfd, struct sockaddr_in *receiverAddr,unsigned char *messageStr, const short int port,int BFL) {
  (*receiverAddr).sin_family = AF_INET;
  (*receiverAddr).sin_port = htons(port);
  (*receiverAddr).sin_addr.s_addr = INADDR_ANY;
  sendto(*sockfd, (const unsigned char *)messageStr,BFL, 0, (const struct sockaddr *)receiverAddr, sizeof(struct sockaddr_in));
}


int main(int argc, char **argv) {

  if (argc == 3) {
    BUFLEN = atoi(argv[2]) + sizeof(short int)+sizeof(int);
    FILE *f;
    int sockfd;
    unsigned char ch,buf[BUFLEN];
    struct sockaddr_in receiverAddr;
    memset(&receiverAddr, 0, sizeof(receiverAddr));
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("Socket Failed\n");
      exit(EXIT_FAILURE);
    }
    
     while(1){
		f = fopen(argv[1],"rb+");
		fseek(f,0,SEEK_SET);
		ch = fgetc(f);
		if(ch == '0'){
			ch = '1';
			fseek(f,1,SEEK_SET);
			fread(buf,sizeof(unsigned char),BUFLEN,f);
			fseek(f,0,SEEK_SET);
			fwrite(&ch,1,1,f);
          		sendmessage(&sockfd, &receiverAddr, buf + sizeof(short int)+sizeof(int), *((short int *)buf),*((int *)(buf+sizeof(short int))));
		}
		fclose(f);
       }

  } else {
    printf("Too few arguments given : mouth \n");
  }

  return 0;
}
