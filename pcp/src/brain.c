#include "ecdc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "brainutil.h"
// TODO  Reciever Table 
// TODO  Post buffer send routine
// TODO  Reciever Routine
// TODO  nRecord Table sharing routine 
// TODO  Difference computation routine
sysInfo *sysinfo;
senderRecord* senderTable;
senderRecord *temp,*pointer;
int numrecords=0;
char timeFlag = 1;
static long sysTime = 0;
char *bf;
FILE *f, *be, *uio;
int enew = 0, eold =0;
void sendToFile(char *fname, char *buf, int size) {
  f = fopen(fname, "ab");
  fwrite(buf, 1, size, f);
  fclose(f);
}
void sys_tick() {
  while (timeFlag) {
    senderRecord* temp = senderTable,*prev;
	  do{
	    prev = temp;
	    temp = temp->next;
	    if(temp->numTicks != 0){
		    temp->numTicks--;
        printf("type : %d status : %d sid : %s numticks : %ld mid : %s message: %s\n",temp->type,temp->status,(temp->nr)->sid,temp->numTicks,temp->messageId.output,temp->message);
	    }else{
		    continue;
	    }
	  }while(temp != senderTable);
    sysTime = (sysTime % 0xFFFFFFFFFFFFFFFF) + 1;
    usleep(200);
  }
}

void createSysMessage(char, nRecord*, unsigned char*, int, char*,result);
void printdecon(deconSys);
deconSys convertSysMessage(char*);
void bufferExchng(char*);
deconSys getFromEar();
result generateMsgId();
void deleteByMsgId(int messageId);
char doesExistMsgId(int messageId,char type);
char doesExistbyTo(short int from,char type);
nRecord* getRecordByPort(short int from);
nRecord* getRecordBySid(char* sid);
nRecord* createRecord();
void checkStateAndProcess();
int main(int argc, char **argv) {
  // load config
    srand(0);
    result t;
    deconSys iMsg;
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
                sysinfo->recordTable[recordIt].buffer = DEFAULT_BUFFER;
                sysinfo->recordTable[recordIt].status = '?';
                sysinfo->recordTable[recordIt].numTicks = DEFAULT_TICKS;
                recordIt += 1;
              }
            }
          }
	senderTable = createSenderRecord(-1,0,NULL,0,t,NULL);
	senderTable->next = senderTable;
	pointer = senderTable;
  	bf = (char*)malloc(sysinfo->sysBuffer + sizeof(short int));
  	clm(bf);
          i += 1;
          j = 0;
          line = (char *)malloc(20);
        } else {
          j += 1;
        }
      }

      while (((be = fopen(intfiles[0], "rb")) == NULL) ||
             ((uio = fopen(intfiles[2], "rb")) == NULL))
        continue;
      printf("TEST2\n");
      fseek(be, 0, SEEK_END);
      fseek(uio, 0, SEEK_END);
      int unew = ftell(uio), uold = ftell(uio);
      enew = ftell(be);
      eold = ftell(be);
      int dsize = 0,offset=0;
      result res;
      nRecord *sender;
      // *((short int *)bf) = sysinfo->recordTable[0].port;
      // sprintf((bf + sizeof(short int)), "Hello world this is a text message
      // ");
      // buffer info exchangement
      // Peer information exchangement
      // Brain will start
      bufferExchng(intfiles[1]);
      while (1) {
        iMsg = getFromEar();
        if(iMsg.type != -1){
            switch(iMsg.type){
                case 0:
                    deleteByMsgId(iMsg.messageId);
                    break;
                case 1:
                    if(1 == doesExistMsgId(iMsg.messageId,2)){
                        sender = getRecordByPort(iMsg.from);
                        sender->buffer = *((int*)(iMsg.data));
                        // send ok
                        createSysMessage(0,sender,"",0,intfiles[1],encoder((unsigned char*)&iMsg.messageId,sizeof(int)));
                        // TODO delete message of this messageId
                        deleteByMsgId(iMsg.messageId);
                    }
                    break;
                case 2:
                        if(1 == doesExistbyTo(iMsg.from,2)){

                        }else{
                            res = decoder(iMsg.data);
                            offset += res.numByte;
                            sender = getRecordBySid(res.output);
                            sender->port = iMsg.from;
                            sender->buffer = *((int*)(iMsg.data+offset));
                            sender->buffer = (sysinfo->sysBuffer<=sender->buffer)?sysinfo->sysBuffer:sender->buffer;
                            sender->status = 1;
                            sender->numTicks = DEFAULT_TICKS;
                            // send 1	
                            createSysMessage(1,sender,(unsigned char*)(&sender->buffer),sizeof(int),intfiles[1],encoder((unsigned char*)&iMsg.messageId,sizeof(int)));
                        }
                    break;
            }
      }else{}
        // checkStateAndProcess();
      }
    }
  } else {
    printf("Too few arguments brain: \n");
  }
  return 0;
}
result generateMsgId(){
	int id=1000+rand()%9000;
    return encoder((unsigned char*)&id,sizeof(int));
}
void createSysMessage(char type,nRecord *nr, unsigned char *data,int dsize,char* intfiles,result messageId) {
  int offset = 0, i = 0,size ;
  result res;
  res = encoder(nr->sid, strlen(nr->sid));
  size = sizeof(int) + sizeof(short int) * 2 + res.numByte + dsize + sizeof(char)+messageId.numByte;
  clm(bf);
  if (nr->buffer >= size) {
    *((short int *)bf) = nr->port;
    offset += sizeof(short int);
    *((int *)(bf + offset)) = size;
    offset += sizeof(int);
    *(bf + offset) = type;
    offset += sizeof(char);
    *((short int *)(bf + offset)) = nr->port;
    offset += sizeof(short int);
    *((short int *)(bf + offset)) = sysinfo->port;
    offset += sizeof(short int);
    i = 0;
    while(i < messageId.numByte){
        *(bf + offset + i) = messageId.output[i];
        i += 1;
    }
    offset += messageId.numByte;
    i = 0;
    while (i < res.numByte) {
      *(bf + offset + i) = res.output[i];
      i += 1;
    }
    offset += res.numByte;
    i = 0;
    while (i < dsize) {
      *(bf + offset + i) = data[i];
      i += 1;
    }
    createSenderRecord(type,3,nr,nr->numTicks,messageId,data);
    sendToFile(intfiles, bf, sysinfo->sysBuffer + sizeof(short int));
  } else{
     
   }
}
void printdecon(deconSys ds){
	result res;
	res = decoder(ds.data);
	printf("\ntype : %d\n",ds.type);
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
    res = decoder((unsigned char*)(buffer+offset));
    output.messageId = *((int*)res.output);
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
  deconSys iMsg;
	res = encoder(sysinfo->systemId,strlen(sysinfo->systemId));
	res.output = (unsigned char*)realloc(res.output,(res.numByte+sizeof(int)));
	*((int*)(res.output+res.numByte)) = sysinfo->sysBuffer;
	while(i < sysinfo->numRecords){
        createSysMessage(2,&sysinfo->recordTable[i],res.output,(res.numByte+sizeof(int)),intfiles,generateMsgId());
		i += 1;
	}
}
deconSys getFromEar(){
  int dsize,i;
  deconSys output;
  output.type = -1;
  fseek(be, 0, SEEK_END);
  enew = ftell(be);
  dsize = enew - eold;
  if (dsize > 0) {
    fseek(be, -dsize, SEEK_END);
    clm(bf);
    fread(bf,sysinfo->sysBuffer, 1, be);
    output =(convertSysMessage(bf));
    eold += sysinfo->sysBuffer;
  }
  return output;   
}
//  void breakMsg(nRecord target, char* data, int dsize, char* bf){
//  if(target.buffer > dsize){
//        createSysMessage(2, sysinfo->port, target.port, target.sid, data, dsize, bf);
//  } else{
//  
//  }
void deleteByMsgId(int messageId){
  senderRecord* temp = senderTable,*prev;
	do{
	  prev = temp;
	  temp = temp->next;
	  if(*((int*)((temp->messageId).output)) == messageId){
		  prev->next = temp->next;
      free(temp);
	  }else{
		  continue;
	  }  
    }while(temp != senderTable);

}
char doesExistMsgId(int messageId,char type){
  senderRecord* temp = senderTable,*prev;
  do{
	  prev = temp;
	  temp = temp->next;
	  if(*((int*)((temp->messageId).output)) == messageId && temp->type == type){
      return 1;
	  }else{
		  continue;
	  }
    }while(temp != senderTable);
    return 0;
}
nRecord* getRecordByPort(short int from){
  int i = 0;
  while(i < sysinfo->numRecords){
    if((sysinfo->recordTable[i]).port == from){
      return &sysinfo->recordTable[i];
    }
  }
  return NULL;
}
nRecord* createRecord(){
	sysinfo->numRecords += 1;
	sysinfo->recordTable = (nRecord*)realloc(sysinfo->recordTable,sizeof(nRecord)*sysinfo->numRecords);
	return sysinfo->recordTable+(sysinfo->numRecords)-1;
}
nRecord* getRecordBySid(char* sid){
  int i = 0;
  while(i < sysinfo->numRecords){
    if(!strcmp((sysinfo->recordTable[i]).sid ,sid)){
      return &sysinfo->recordTable[i];
    }
  } 

  return createRecord();
}
char doesExistbyTo(short int from,char type){
  senderRecord* temp = senderTable,*prev;
  do{
	  prev = temp;
	  temp = temp->next;
	  if((temp->nr)->port == from && temp->type == type){
      return 1;
	  }else{
		  continue;
	  }
    }while(temp != senderTable);
    return 0;
}
void checkStateAndProcess(){
  senderRecord* temp = senderTable,*prev;
  do{
//   type; status; numTicks; result messageId;
	  prev = temp;
	  temp = temp->next;
	  if((0 == temp->numTicks) && (0 == status)){
	 	prev->next = temp->next;
	  }else if((0==temp->numTicks)){
		 // send the message again
		 // reset the numTicks
		 // decrement the status 
	  }else{
	  	continue;
	  }
    }while(temp != senderTable);
}
