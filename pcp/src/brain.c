#include "ecdc.h"
#include "brainutil.h"
#include <time.h>
// TODO  Difference computation routine
// TODO  nRecord Table sharing routine 

static long sysTime = 0;
sysInfo *sysinfo;
senderRecord* senderTable;
senderRecord *temp,*pointer;
int numRecords=0;
char timeFlag = 1;
char *bf;
FILE *f, *be, *uio;
char *intfiles[4];
int enew = 0, eold =0;


void sendToFile(char *fname, char *buf, int size) {
  f = fopen(fname, "ab");
  fwrite(buf, 1, size, f);
  fclose(f);
}
void sys_tick() {
  while (timeFlag) {
    senderRecord* temp = senderTable,*prev;
    prev = temp;
    temp = temp->next;
    while(temp != senderTable){
	 if(temp->numTicks != 0){
             temp->numTicks--;
//              printf("type : %d status : %d sid : %s numticks : %ld mid : %s message: %s\n",temp->type,temp->status,(temp->nr)->sid,temp->numTicks,temp->messageId.output,temp->message);
 	 }
    	prev = temp;
    	temp = temp->next;
     }
    sysTime = (sysTime % 0xFFFFFFFFFFFFFFFF) + 1;
    usleep(5000);
  }
}

void pst(){
   senderRecord* temp = senderTable;
   senderRecord *prev;
   prev = temp;
   temp = temp->next;
   while((temp != senderTable)){
           //printf("%d %s %d \n",temp->type,temp->nr->sid,temp->nr->port);
           printf("%d %s %d\n",temp->type,sysinfo->recordTable[temp->nr].sid,temp->messageId);
 	  prev = temp;
 	  temp = temp->next;
}
}
int main(int argc, char **argv) {
     time_t t;
     srand((unsigned)time(&t));
  // load config
    deconSys iMsg;
    if (argc == 2) {
    FILE *config = fopen(argv[1], "r");
    int linesRead = 0;
    int i = 0, j = 0, recordIt = 0;
    char *line;
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
	senderTable = createSenderRecord(-1,0,-1,0,-1,NULL);
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
      fseek(be, 0, SEEK_END);
      fseek(uio, 0, SEEK_END);
      int unew = ftell(uio), uold = ftell(uio);
      enew = ftell(be);
      eold = ftell(be);
      int dsize = 0,offset=0;
      result res;
      int sender;
      // *((short int *)bf) = sysinfo->recordTable[0].port;
      // sprintf((bf + sizeof(short int)), "Hello world this is a text message
      // ");
      // buffer info exchangement
      // Peer information exchangement
      // Brain will start
      bufferExchng(); 
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
                        sysinfo->recordTable[sender].buffer = *((int*)(iMsg.data));
                        // send ok
                        deleteByMsgId(iMsg.messageId);
                        createSysMessage(0,DEFAULT_STATUS,sender,"OK",2,iMsg.messageId);
                    	deleteByMsgId(iMsg.messageId);
                        // TODO delete message of this messageId
                    }
                    break;
                case 2:
                        if(1 == doesExistbyTo(iMsg.from,2)){
		    	    printf("iCASE 2 : %s \n",sysinfo->systemId);
                        }else{
                            res = decoder(iMsg.data);
                            offset = res.numByte;
                            sender = getRecordBySid(res.output);
			    sysinfo->recordTable[sender].sid=res.output;
                            sysinfo->recordTable[sender].port = iMsg.from;
			    sysinfo->recordTable[sender].buffer = *((int*)(iMsg.data+offset));
                            sysinfo->recordTable[sender].buffer = (sysinfo->sysBuffer<=sysinfo->recordTable[sender].buffer)?sysinfo->sysBuffer:sysinfo->recordTable[sender].buffer;
                            sysinfo->recordTable[sender].status = 1;
                            sysinfo->recordTable[sender].numTicks = DEFAULT_TICKS;
                            // send 1	
                            createSysMessage(1,DEFAULT_STATUS,sender,(unsigned char*)(&sysinfo->recordTable[sender].buffer),sizeof(int),iMsg.messageId);
                        }
                    break;
            }
      }else{}
       checkStateAndProcess();
      }
    }
  } else {
    printf("Too few arguments brain: \n");
  }
  return 0;
}

int generatemsgid(){
	int id=1000+(rand())%9000;
    return id;
}
void createSysMessage(char type,char status,int index, unsigned char *data,int dsize,int mId) {
  int offset = 0, i = 0,size ;
  result res,messageId;
  messageId = encoder((unsigned char*)&mId,sizeof(int));
nRecord* nr = &sysinfo->recordTable[index];
  res = encoder(nr->sid, strlen(nr->sid));
  size = sizeof(int) + sizeof(short int) * 2 + res.numByte + dsize + sizeof(char)+messageId.numByte;
  clm(bf);
  if (nr->buffer >= size) {
    if(type == 0){
}
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
   messageId = decoder(bf+(offset-messageId.numByte));
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
printf("S--------------\n");
    addToSenderTable(type,status,index,nr->numTicks,mId,data);
    pst();
printf("E--------------\n");
    sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int));
  } else{
     
   }
}
void printdecon(deconSys ds){
	result res;
	res = decoder(ds.data);
	printf("\ntype : %d\n",ds.type);
	printf("from : %d\n",ds.from);
	printf("to : %d\n",ds.to);
	printf("sysid : %s\n",ds.sysId);
	printf("data : %s\n",res.output);
	printf("num byte : %d\n",(res.numByte));
	printf("buffer size : %d\n",*((int*)(ds.data+res.numByte)));
}
deconSys convertSysMessage(char *buffer){
deconSys output;
result res;
int offset = 0,i=0,dsize=0;
if(*((int*)buffer) <= sysinfo->sysBuffer){
	offset += sizeof(int);
	output.type = *(buffer + offset) ;
    	offset += sizeof(char);
	output.to= *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
	output.from = *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
        res = decoder((unsigned char*)(buffer+offset));
        output.messageId = *((int*)res.output);
        offset += res.numByte;
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
void bufferExchng() {
	// for all knowns create system msg for buffer sending
	int i =0;
	result res;
  	deconSys imsg;
	res = encoder(sysinfo->systemId,strlen(sysinfo->systemId));
	res.output = (unsigned char*)realloc(res.output,(res.numByte+sizeof(int)));
	*((int*)(res.output+res.numByte)) = sysinfo->sysBuffer;
	while(i < sysinfo->numRecords){
        createSysMessage(2,DEFAULT_STATUS,i,res.output,(res.numByte+sizeof(int)),generatemsgid());
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
  if (dsize > 0 && 0 == (dsize % sysinfo->sysBuffer)) {
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
  	senderRecord *prev,*t;
	t = senderTable;
 	prev = t;
 	t = t->next;
	while(t != senderTable){
	 if(t->messageId == messageId){
		 prev->next = t->next;
		 // free the space 
	 }
	 prev = t;
	 t = t->next;
 }

}
char doesExistMsgId(int messageId,char type){
  char output = 0;
  senderRecord* prev,*t;
  t = senderTable;
  prev = t;
  t = t->next;
  while(t != senderTable && 0 == output){
	 if(t->messageId == messageId && t->type == type){
	 output = 1;
	 } 
	 prev = t;
	 t = t->next;
	}
    return output;
}
int getRecordByPort(short int from){
  int i = 0;
  while(i < sysinfo->numRecords){
    if((sysinfo->recordTable[i]).port == from){
      return i;
    }
i+=1;
  }
  return -1;
}
int createRecord(){
	sysinfo->numRecords += 1;
sysinfo->recordTable = (nRecord*)realloc(sysinfo->recordTable,sizeof(nRecord)*sysinfo->numRecords);
	return ((sysinfo->numRecords)-1);
}
int getRecordBySid(char* sid){
  int i = 0;
  while(i < sysinfo->numRecords){
    if(!strcmp((sysinfo->recordTable[i]).sid ,sid)){
      return i;
    }
i+=1;
  } 

  return createRecord();
}
char doesExistbyTo(short int from,char type){
  senderRecord* t = senderTable,*prev;
  char output = 0;
  prev = t;
  t = t->next;
  while((t != senderTable)&&(0 == output)){
	  if((sysinfo->recordTable[t->nr]).port == from && t->type == type){
      		output = 1;
	  }
	  prev = t;
	  t = t->next;
	}
    return output;
}
void checkStateAndProcess(){
 	senderRecord* prev,*t;
	t = senderTable;
	prev = t;
	t = t->next;
	while(t != senderTable){
			// free the space in both if else
		  if((0 == t->numTicks) && (0 == t->status)){
			// mark the target system dead
	 		prev->next = t->next;
	 	   }else if((0==t->numTicks)){
        		createSysMessage(t->type,(t->status-1),t->nr,t->message,strlen(t->message),t->messageId);
	 		prev->next = t->next;
               	   }
	  prev = t;
	  t = t->next;
    }
}
