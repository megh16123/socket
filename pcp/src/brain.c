#include "ecdc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
typedef struct {
  char *sid;
  short int port;
  int buffer;
  char status;
} nRecord;
typedef struct{
 char type;
 short int from;
 short int to;
 unsigned char *sysId;
 unsigned char *data;
}deconSys;

typedef struct {
  unsigned char *systemId;
  short int port;
  int numRecords;
  nRecord *recordTable;
  int sysBuffer;
} sysInfo;
sysInfo *sysinfo;

char timeFlag = 1;
static long sysTime = 0;
char *bf;
FILE *f;
void sendToFile(char *fname, char *buf, int size) {
  f = fopen(fname, "ab");
  fwrite(buf, 1, size, f);
  fclose(f);
}
void sys_tick() {
  while (timeFlag) {
    sysTime = (sysTime % 0xFFFFFFFFFFFFFFFF) + 1;
    usleep(200);
  }
}
void createSysMessage(char type, short int from, short int to, unsigned char *sysId, unsigned char *data,int dsize, char *buffer, char*intfiles) {
  int offset = 0, i = 0,size ;
  result res;
  res = encoder(sysId, strlen(sysId));
  size = sizeof(int) + sizeof(short int) * 2 + res.numByte + dsize + sizeof(char);
  if (sysinfo->sysBuffer >= size) {
    *((short int *)buffer) = to;
    offset += sizeof(short int);
    *((int *)(buffer + offset)) = size;
    offset += sizeof(int);
    *(buffer + offset) = type;
    offset += sizeof(char);
    *((short int *)(buffer + offset)) = from;
    offset += sizeof(short int);
    *((short int *)(buffer + offset)) = to;
    offset += sizeof(short int);
    while (i < res.numByte) {
      *(buffer + offset + i) = res.output[i];
      i += 1;
    }
    offset += res.numByte;
    i = 0;
    while (i < dsize) {
      *(buffer + offset + i) = data[i];
      i += 1;
    }
    sendToFile(intfiles, bf, sysinfo->sysBuffer + sizeof(short int));
  } else{
     
   }
}
void printdecon(deconSys ds){
	result res;
	res = decoder(ds.data);
	printf("type : %d\n",ds.type);
	printf("from : %d\n",ds.from);
	printf("to : %d\n",ds.to);
	printf("sysId : %s\n",ds.sysId);
	printf("data : %s\n",res.output);
	printf("NUM BYTE : %d\n",(res.numByte));
	printf("BUFFER SIZE : %d\n",*((int*)(ds.data+res.numByte)));
}
deconSys convertSysMessage(char *buffer){
deconSys output;
result res;
int offset = 0,i=0,dsize=0;
if(*((int*)buffer) <= sysinfo->sysBuffer){
	offset += sizeof(int);
	output.type = *(buffer + offset) ;
    	offset += sizeof(char);
	output.from = *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
	output.to = *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
	res = decoder((unsigned char*)(buffer+offset));
	output.sysId = res.output;
	offset += res.numByte;
	dsize = (*((int*)buffer)-offset);	
	output.data = (unsigned char*)malloc(dsize);
	while(i < dsize){
		output.data[i] = *(buffer+offset+i);
		i += 1;
	}
}/*else{}*/
return output;
}
void bufferExchng(char *intfiles) {
	// for all knowns create system msg for buffer sending
	int i =0;
	result res;
	res = encoder(sysinfo->systemId,strlen(sysinfo->systemId));
	res.output = (unsigned char*)realloc(res.output,(res.numByte+sizeof(int)));
	*((int*)(res.output+res.numByte)) = sysinfo->sysBuffer;
	while(i < sysinfo->numRecords){
		clm(bf);	
      		createSysMessage(2, sysinfo->port, sysinfo->recordTable[i].port, sysinfo->recordTable[i].sid,res.output,(res.numByte+sizeof(int)), bf,intfiles);
		i += 1;
	}
	// push to bm
	// start timer
	// wait for feedback
	// if feedback is there then update the buffer to the minimum of the systems
	// if feedback is not there then again buffer exchange will happen after the timerexpires
	// mark the system down if there is no response again

}
//  void breakMsg(nRecord target, char* data, int dsize, char* bf){
//  if(target.buffer > dsize){
//        createSysMessage(2, sysinfo->port, target.port, target.sid, data, dsize, bf);
//  } else{
//  
//  }
//  }
int main(int argc, char **argv) {
  // load config
  if (argc == 2) {
    FILE *config = fopen(argv[1], "r");
    int linesRead = 0;
    int i = 0, j = 0, recordIt = 0;
    char *intfiles[4], *line;
    pthread_t tmp;
    sysinfo = (sysInfo *)malloc(sizeof(sysInfo));
    pthread_create(&tmp, NULL, (void *)&sys_tick, NULL);
    pthread_detach(tmp);
    if (NULL == config) {
      printf("File didn't open \n");
    } else {
      line = (char *)malloc(20);
      while ((line[j] = fgetc(config)) != EOF) {
        if (line[j] == '\n') {
          line[j] = '\0';
          linesRead += 1;
          if (1 == linesRead) {
            sysinfo->systemId = line;
          } else if (2 == linesRead) {
            sysinfo->port = (short int)atoi(line);
          } else if (3 == linesRead) {
            sysinfo->sysBuffer = atoi(line);
          } else if (4 <= linesRead && 7 >= linesRead) {
            intfiles[linesRead - 4] = line;
          } else if (8 == linesRead) {
            sysinfo->numRecords = atoi(line);
            sysinfo->recordTable =
                (nRecord *)malloc(sysinfo->numRecords * sizeof(nRecord));
          } else {
            if (sysinfo->numRecords > recordIt) {

              if (0 != (linesRead % 2)) {
                sysinfo->recordTable[recordIt].sid = line;
              } else {
                sysinfo->recordTable[recordIt].port = (short int)atoi(line);
                sysinfo->recordTable[recordIt].buffer = -1;
                sysinfo->recordTable[recordIt].status = '?';
                recordIt += 1;
              }
            }
          }
  	bf = (char*)malloc(sysinfo->sysBuffer + sizeof(short int));
  	clm(bf);
          i += 1;
          j = 0;
          line = (char *)malloc(20);
        } else {
          j += 1;
        }
      }

      FILE *be, *bm, *uio, *uii;
      while (((be = fopen(intfiles[0], "rb")) == NULL) ||
             ((uio = fopen(intfiles[2], "rb")) == NULL))
        continue;
      printf("TEST2\n");
      fseek(be, 0, SEEK_END);
      fseek(uio, 0, SEEK_END);
      int unew = ftell(uio), uold = ftell(uio), enew = ftell(be),
          eold = ftell(be), dsize = 0;

      // *((short int *)bf) = sysinfo->recordTable[0].port;
      // sprintf((bf + sizeof(short int)), "Hello world this is a text message
      // ");
      // buffer info exchangement
      // Peer information exchangement
      // Brain will start
      bufferExchng(intfiles[1]);
      while (1) {
        fseek(be, 0, SEEK_END);
        enew = ftell(be);
        dsize = enew - eold;
        if (dsize > 0) {
          fseek(be, -dsize, SEEK_END);
          dsize = dsize / sysinfo->sysBuffer;
          for (int i = 0; i < dsize; i++) {
            clm(bf);
            fread(bf,sysinfo->sysBuffer, 1, be);
	    printdecon(convertSysMessage(bf));
            eold += (i + 1) * sysinfo->sysBuffer;
          }   
        }
      }
    }
  } else {
    printf("Too few arguments brain: \n");
  }
  return 0;
}
