#include "ecdc.h"
#include "brainutil.h"
#include <time.h>

// TODO  Difference computation routine
// TODO  nRecord Table sharing routine 
// TODO  Reciever end's timers need to be written
// TODO  Assemble Everything in case 


static long sysTime = 0;
sysInfo *sysinfo;
senderRecord* senderTable;
senderRecord *temp,*pointer;
recieverRecord* recieverTable;
recieverRecord *rtemp,*rpointer;
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
 	 }
    	prev = temp;
    	temp = temp->next;
     }
    
    recieverRecord* rtemp = recieverTable,*rprev;
    rprev = rtemp;
    rtemp = rtemp->next;
    while(rtemp != recieverTable){
	 if(rtemp->numTicks != 0){
             rtemp->numTicks--;
 	 }
    	rprev = rtemp;
    	rtemp = rtemp->next;
     }

    sysTime = (sysTime % 0xFFFFFFFFFFFFFFFF) + 1;
    usleep(900);
  }
}

void printRecordTable(){
	int i=0;
        printf("---------------------------------\n");
   	printf("  ID\t\tPort\tBuffer\n");
	while(i<sysinfo->numRecords)
	{
		printf("%s\t\t%d\t%d\n",sysinfo->recordTable[i].sid,sysinfo->recordTable[i].port,sysinfo->recordTable[i].buffer);i++;
	}
   printf("---------------------------------\n");
}

void prt(){
   recieverRecord* temp = recieverTable;
   recieverRecord *prev;
   prev = temp;
   temp = temp->next;
   printf("--------------Reciever-----------------\n");
   printf("  mesgID\tType\tFrom\tVector\n");
   while((temp != recieverTable)){
           printf("  %d\t\t%d\t%d\t%b\n",temp->messageId,temp->type,temp->from,*(temp->bv));
 	  prev = temp;
 	  temp = temp->next;
}
   printf("---------------------------------------\n");
}

void pst(){
   senderRecord* temp = senderTable;
   senderRecord *prev;
   prev = temp;
   temp = temp->next;
   printf("-------------Sender--------------\n");
   printf("  mesgID\tType\tTo\n");
   while((temp != senderTable)){
           printf("  %d\t\t%d\t%s\n",temp->messageId,temp->type,sysinfo->recordTable[temp->nr].sid);
 	  prev = temp;
 	  temp = temp->next;
}
   printf("---------------------------------\n");
}

int main(int argc, char **argv) {
     time_t t;
     srand((unsigned)time(&t));

  // load config
    deconSys iMsg;
    if (argc == 2) {
    FILE *config = fopen(argv[1], "r");
    int linesRead = 0;
    int i = 0, j = 0, recordIt = 0,cs;
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
	senderTable = createSenderRecord(-1,0,-1,0,-1,NULL,0);
	recieverTable = createRecieverRecord(-1,0,-1,0,-1,NULL,0);
	senderTable->next = senderTable;
	recieverTable->next = recieverTable;
	pointer = senderTable;
	rpointer = recieverTable;
  	
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
      bufferExchng(); 
      while (1) {
        iMsg = getFromEar();
        if(iMsg.type != -1){
            switch(iMsg.type){
                case 0:
                    createSysMessage(5,DEFAULT_STATUS,0,"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",5120,iMsg.messageId+1);
                    deleteByMsgId(iMsg.messageId);
                    deleteByMsgId(iMsg.messageId+1);
                    break;
                case 1:
                    if(1 == doesExistMsgId(iMsg.messageId,2)){
                        sender = getRecordByPort(iMsg.from);
                        sysinfo->recordTable[sender].buffer = *((int*)(iMsg.data));
                        // send ok
                        deleteByMsgId(iMsg.messageId);
                        createSysMessage(0,DEFAULT_STATUS,sender,"OK",2,iMsg.messageId);
                    	deleteByMsgId(iMsg.messageId);
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
	case 5:
		//printf("CASE 5 %d from %d\n",iMsg.messageId,iMsg.from);
		recieverRecord* rec = rdoesExistByMsgId(iMsg.messageId,5);
		sender = getRecordByPort(iMsg.from);
		cs = iMsg.size - 2; 
		printf("index: %d\n",*(iMsg.data+1));
		if(NULL != rec){
			*(rec->bv + (((int)(*(iMsg.data+1))) / 8)) |= mask((((int)(*(iMsg.data+1))) % 8));
			rec->numTicks = DEFAULT_TICKS;
		i = 0;
		while(i < cs){
			*(rec->data + cs*(*(iMsg.data+1))+i) = *(iMsg.data+2+i);
			i+=1;
			}
		i = 0;
		}else{
			if((*(iMsg.data)-1) == *(iMsg.data + 1)){
				addToRecieverTable(5,3,iMsg.from,DEFAULT_TICKS,iMsg.messageId,(char*)malloc((sysinfo->recordTable[sender].buffer-(16+ceil(((strlen(sysinfo->systemId)*9)+1)/8.0)))*(*(iMsg.data))),*(iMsg.data));
			}
			else{
				addToRecieverTable(5,3,iMsg.from,DEFAULT_TICKS,iMsg.messageId,(char*)malloc(cs*(*(iMsg.data))),*(iMsg.data));
			}
			i = 0;
			while(i < cs){
				*(rpointer->data + cs*(*(iMsg.data+1))+i) = *(iMsg.data+2+i);
				i+=1;
		}
			i = 0;
			*(rpointer->bv + (((int)(*(iMsg.data+1))) / 8)) |= mask((((int)(*(iMsg.data+1))) % 8));
		//	and add data to table  
		}
		prt();
		break;
            }
       //printRecordTable();
      }else{}
       checkStateAndProcess();
      }
    }
  } else {
    printf("Too few arguments brain: \n");
  }
  return 0;
}
void clear(char *str)
{
	int i=0;
	while(str[i]!='\0')
	{
		str[i]=0;
		i++;
	}

}

int generatemsgid(){
	int id=1000+(rand())%9000;
    return id;
}
void createSysMessage(char type,char status,int index, unsigned char *data,int dsize,int mId) {
    int offset = 0, i = 0, size,cs=0,rem;
    char  noc=0,ic=0;
    result res,messageId;
    messageId = encoder((unsigned char*)&mId,sizeof(int));
    nRecord* nr = &sysinfo->recordTable[index];
    res = encoder(nr->sid, strlen(nr->sid));
    size = sizeof(int) + sizeof(short int) * 2 + res.numByte + sizeof(char)+messageId.numByte;
    clm(bf);
    *((short int *)bf) = nr->port;
    offset += sizeof(short int);
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
    if (nr->buffer >= size+dsize) {
	size += dsize;
    	*((int *)(bf + sizeof(short int))) = size;
    	i = 0;
   	 while (i < dsize) {
      		*(bf + offset + i) = data[i];
      		i += 1;
    	}		
    	sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int));
    } else{
	cs = nr->buffer - (16 + res.numByte);	
	noc = ceil(dsize/(double)cs);
	i = 0;
	*(bf+offset) = noc;
	offset += 1;
	while(ic < noc){
    	 i = 0;
	// clear(bf+offset);
	 clm(bf+offset);
	 *(bf+offset) = ic;
	 if(ic==noc-1){
		rem=dsize%cs;
		size = (rem+2);
    		*((int *)(bf + sizeof(short int))) = size;
		while (i < rem) {
      		*(bf + offset + 1 + i) = data[(((int)ic)*cs)+i];
      		i += 1;
		}
	} else{ 
		size = (cs+2);
    		*((int *)(bf + sizeof(short int))) = size;
		while (i < cs) {
      		*(bf + offset + 1 + i) = data[(((int)ic)*cs)+i];
      		i += 1;
		}
	}		
	ic += 1;
    	sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int));
	}
	printf(" CS : %d NOC : %d\n",cs,noc);	
    }
    addToSenderTable(type,status,index,nr->numTicks,mId,data,noc);
    pst();
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
output.size = *((int*)buffer);
if(*((int*)buffer) <= sysinfo->sysBuffer){
	offset += sizeof(int);
	output.size -= sizeof(int);
	output.type = *(buffer + offset) ;
    	offset += sizeof(char);
	output.size -= sizeof(char);
	output.to= *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
	output.size -= sizeof(short int);
	output.from = *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
	output.size -= sizeof(short int);
        res = decoder((unsigned char*)(buffer+offset));
        output.messageId = *((int*)res.output);
        offset += res.numByte;
	output.size -= res.numByte;
	res = decoder((unsigned char*)(buffer+offset));
	output.sysId = res.output;
	offset += res.numByte;
	output.size -= res.numByte;
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

recieverRecord* rdoesExistByMsgId(int messageId,char type)
{ 
  recieverRecord *output=NULL;
  recieverRecord* prev,*t;
  t = recieverTable;
  prev = t;
  t = t->next;
  while(t != recieverTable && output==NULL ){
	 if(t->messageId == messageId && t->type == type){
	 output = t;
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

	recieverRecord* rprev,*rt;
	rt = recieverTable;
	rprev = rt;
	rt = rt->next;
	while(rt != recieverTable){
			// free the space in both if else
		  if((0 == rt->numTicks) && (0 == rt->status)){
			// mark the target system dead
	 		rprev->next = rt->next;
	 	   }else if((0==rt->numTicks)){
        		createSysMessage(rt->type,(rt->status-1),rt->nr,rt->message,strlen(rt->message),rt->messageId);
	 		rprev->next = rt->next;
               	   }
	  rprev = rt;
	  rt = rt->next;
	}
}
