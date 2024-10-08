#include "ecdc.h"
#include "brainutil.h"
#include <time.h>

#define pf(a) logs = fopen(id,"a"); fprintf a ; fclose(logs);
#define DEBUG pf((logs,"\nMesg Came\n"));
#define SYS_MSG 0x80 
#define LRG_MSG	0x40
#define BUFF_MSG 0x01
#define BUFF_REPLY_MSG 0x02
#define PEER_MSG 0x03
#define OK 0x00
#define B_CLEAR 0x3F
#define NOP 255

static long sysTime = 0;
sysInfo *sysinfo;
senderRecord* senderTable;
senderRecord *temp,*pointer;
recieverRecord* recieverTable;
recieverRecord *rtemp,*rpointer;
int numRecords=0;
char timeFlag = 1,flg=0;
char *bf,*id;
FILE *f, *be, *uio, *logs,*config;
char *intfiles[4],*sendBv,*dat;
int tsize=0,enew = 0, eold =0,sendBvc = 0,i,j,wp;
result res,bres;



void sendToFile(char *fname, char *buf, int size) {
  f = fopen(fname, "ab");
  fwrite(buf, 1, size, f);
  fclose(f);
}

void sys_tick() {
  while (timeFlag) {
    senderRecord *temp = senderTable;
    senderRecord *prev;
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
	i=0;
        pf((logs,"\n--------------Record-------------\n"));
   	pf((logs,"  ID\t\tPort\tBuffer\n"));
	while(i<(sysinfo->numRecords)){
		pf((logs,"%s\t\t%d\t%d\n",(sysinfo->recordTable[i]).sid,(sysinfo->recordTable[i]).port,(sysinfo->recordTable[i]).buffer));
		i++;
	}
   	pf((logs,"---------------------------------\n"));
}

void prt(){
   recieverRecord* temp = recieverTable;
   recieverRecord *prev;
   prev = temp;
   temp = temp->next;
   pf((logs,"\n--------------Reciever-----------------\n"));
   pf((logs,"  mesgID\tType\tFrom\tVector\n"));
   while((temp != recieverTable)){
          pf((logs,"  %d\t\t%d\t%d\t",temp->messageId,temp->type,temp->from));
          acces(temp->bv,temp->bvc);
 	  prev = temp;
 	  temp = temp->next;
}
   pf((logs,"\n---------------------------------------\n"));
}

void print(senderRecord* temp){
   	pf((logs,"-------------spec----------------\n"));
   	pf((logs,"  mesgID\tType\tTo\n"));
        pf((logs,"  %d\t\t%d\t%s\n",temp->messageId,temp->type,sysinfo->recordTable[temp->nr].sid));
   	pf((logs,"---------------------------------\n"));
}

void pst(){
   senderRecord* temp = senderTable;
   senderRecord *prev;
   prev = temp;
   temp = temp->next;
   pf((logs,"\n-------------Sender--------------\n"));
   pf((logs,"  mesgID\tType\tTo\n"));
   while((temp != senderTable)){
          pf((logs,"  %d\t\t%d\t%s\n",temp->messageId,temp->type,sysinfo->recordTable[temp->nr].sid));
 	  prev = temp;
 	  temp = temp->next;
}
   pf((logs,"---------------------------------\n"));
}

int main(int argc, char **argv){
    time_t t;
    srand((unsigned)time(&t));
    // load config
    deconSys iMsg;
    if (argc == 2){
    	config = fopen(argv[1], "r");
    	int linesRead = 0;
    	i = 0; j = 0;
    	int recordIt = 0,cs;
    	unsigned char* dat,*fname;
    	char *line;
    	pthread_t tmp;
    	sysinfo = (sysInfo *)malloc(sizeof(sysInfo));
    	if (NULL == config) {
      		pf((logs,"File didn't open \n"));
    	} else {
      		line = (char *)malloc(20);
      		while ((line[j] = fgetc(config)) != EOF) {
       			 if (line[j] == '\n') {
          			line[j] = '\0';
          			linesRead += 1;
          			if (1 == linesRead) {
            				sysinfo->systemId = line;
	    				id=strdup(sysinfo->systemId);
	    				strcat(id,"_logs");
	   				 pf((logs,"%s\n",sysinfo->systemId));
         			 } else if (2 == linesRead) {
            				sysinfo->port = (short int)atoi(line);
          			} else if (3 == linesRead) {
           				 sysinfo->sysBuffer = atoi(line);
          			} else if (4 <= linesRead && 7 >= linesRead) {
            				intfiles[linesRead - 4] = line;
          			} else if (8 == linesRead) {
            				sysinfo->numRecords = atoi(line);
            				if(sysinfo->numRecords!=0)
           	 				sysinfo->recordTable = (nRecord *)malloc(sysinfo->numRecords * sizeof(nRecord));
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
          			i += 1;
          			j = 0;
          			line = (char *)malloc(20);
        		}else{
          			j += 1;
        		}
      		}
	pf((logs,"Configs File Read\n"));
	senderTable = createSenderRecord(0,0,-1,0,-1,NULL,0,0);
	recieverTable = createRecieverRecord(0,0,-1,0,-1,NULL,0,0);
	senderTable->next = senderTable;
	recieverTable->next = recieverTable;
	pointer = senderTable;
	rpointer = recieverTable;
      while (((be = fopen(intfiles[0], "rb")) == NULL) ||
             ((uio = fopen(intfiles[2], "rb")) == NULL))
        continue;
      pthread_create(&tmp, NULL, (void *)&sys_tick, NULL);
      pthread_detach(tmp);
      bf = (char*)malloc(sysinfo->sysBuffer + sizeof(short int));
      clm(bf);
      fseek(be, 0, SEEK_END);
      fseek(uio, 0, SEEK_END);
      int unew = ftell(uio), uold = ftell(uio);
      enew = ftell(be);
      eold = ftell(be);
      int dsize = 0,offset=0;
      result res;
      int sender;
      bufferExchng();
      pf((logs,"\nSent Buffer to Knowns\n"));
      while (1) {
        iMsg = getFromEar();
        if(iMsg.type != NOP){
		if(SYS_MSG == (iMsg.type&SYS_MSG)){
			if(LRG_MSG == (iMsg.type&LRG_MSG)){
				recieverRecord* rec = rdoesExistByMsgId(iMsg.messageId,iMsg.type);
				sender = getRecordByPort(iMsg.from);
				cs = iMsg.size - 2; 
				int timp=(sysinfo->recordTable[sender].buffer-(16+ceil(((strlen(sysinfo->systemId)*9)+1)/8.0)));
				if(NULL != rec){
					rec->dsize += cs;
					i = bitCountToIndex(*(iMsg.data+1),rec->bv,rec->bvc);
					*(rec->bv + ((int)i / 8)) |= mask((int)i % 8);
					j = 0;
					while(j < cs){
						*(rec->data + timp*(i)+j) = *(iMsg.data+2+j);
						j+=1;
					}
				}else{
					addToRecieverTable(iMsg.type,DEFAULT_STATUS,iMsg.from,DEFAULT_TICKS,iMsg.messageId,(char*)malloc(timp*(*(iMsg.data))),*(iMsg.data),cs);
					i = 0;
					while(i < cs){
						*(rpointer->data + cs*(*(iMsg.data+1))+i) = *(iMsg.data+2+i);
						i+=1;
					}
					i = 0;
					*(rpointer->bv + (((int)(*(iMsg.data+1))) / 8)) |= mask((((int)(*(iMsg.data+1))) % 8));
				}
				prt();
			}else{
				switch(iMsg.type&B_CLEAR){
				
					case OK:
		  				pf((logs,"Case 0: Recieved OK for Message: %d From: %d\n",iMsg.messageId,iMsg.from));
						if(7 == *(iMsg.data)){
							// send peer table
                  					sender = getRecordByPort(iMsg.from);
							// Create Peer Table 
							i = 0;
							tsize  = 0;
							while(i < sysinfo->numRecords){
								tsize += ceil(((strlen(sysinfo->recordTable[i].sid)*9)+1)/8.0);
								i += 1;
							}	
							i = 0;	
							dat = (unsigned char*)malloc(tsize*sizeof(unsigned char)+sysinfo->numRecords*sizeof(short int));
							wp = 0;
							while(i < sysinfo->numRecords){
								res = encoder(sysinfo->recordTable[i].sid,strlen(sysinfo->recordTable[i].sid));
								j = 0;
								while(j < res.numByte){
									*(dat+wp+j) = res.output[j];	
									j += 1;
								}
								wp += res.numByte;
								*((short int*)(dat+wp)) = sysinfo->recordTable[i].port;
								wp += sizeof(short int);
								i += 1;
							}
                  					narad(SYS_MSG|PEER_MSG,DEFAULT_STATUS,sender,dat,wp,generatemsgid());
		  				}else if(6 == *(iMsg.data)){
							// Schedule the file to be sent
						}
               	  				deleteByMsgId(iMsg.messageId);
						//char ata[5119];
						//i = 0;
						//while(i < 5119) ata[i++] = 'P';
		  				//narad(SYS_MSG|0x04,DEFAULT_STATUS,0,ata,5119,iMsg.messageId+1);
               	  				//deleteByMsgId(iMsg.messageId+1);
						break;	

					case BUFF_MSG:
		    				pf((logs,"\nCase 1: Recieved Buffer From: %d\n",iMsg.from));
                       				 if(1 == doesExistbyTo(iMsg.from,iMsg.type)){
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
                            				narad(SYS_MSG|BUFF_REPLY_MSG,DEFAULT_STATUS,sender,(unsigned char*)(&sysinfo->recordTable[sender].buffer),sizeof(int),iMsg.messageId);
		    	    				pf((logs,"Sent Buffer To: %d\n",iMsg.from));
              	    				}
						 break;
					case BUFF_REPLY_MSG:
		    				pf((logs,"\nCase 2: Recieved reply Buffer From: %d\n",iMsg.from));
                    				if(1 == doesExistMsgId(iMsg.messageId,(SYS_MSG|BUFF_MSG))){
                        				sender = getRecordByPort(iMsg.from);
                        				sysinfo->recordTable[sender].buffer = *((int*)(iMsg.data));
                        				deleteByMsgId(iMsg.messageId);
 							if(0 == flg){
 								flg = 7;
 								narad(SYS_MSG|OK,0,sender,(unsigned char*)&flg,1,iMsg.messageId);
 							}else{
								flg = 0;
                        					narad(SYS_MSG|OK,0,sender,&flg,1,iMsg.messageId);
 							}
							flg = 1;
		    				pf((logs,"Sent OK for Msg: %d To: %d\n",iMsg.messageId,iMsg.from));
                    				}
                    				break;
					case PEER_MSG:
		        			pf((logs,"\nCase 7: Recieved Large Peer Info From: %d\n",*((short int*)(iMsg.data+7))));
						wp = 0;
						bres = encoder(sysinfo->systemId,strlen(sysinfo->systemId));
						bres.output = (unsigned char*)realloc(bres.output,(bres.numByte+sizeof(int)));
						*((int*)(bres.output+bres.numByte)) = sysinfo->sysBuffer;
						i = 0;
						tsize  = 0;
						while(i < sysinfo->numRecords){
							tsize += ceil(((strlen(sysinfo->recordTable[i].sid)*9)+1)/8.0);
							i += 1;
						}
						i = 0;
						dat = (unsigned char*)malloc(tsize*sizeof(unsigned char)+sysinfo->numRecords*sizeof(short int));
						sendBvc=ceil(sysinfo->numRecords/8.0);
						sendBv=(unsigned char*)calloc(sendBvc,sizeof(unsigned char));
						tsize = sysinfo->numRecords;
						while(wp < iMsg.size){
							res = decoder(iMsg.data+wp);
							wp += res.numByte;
							i=searchRecordByPort(*((short int*)(iMsg.data+wp)));
							if(i!=-1){
								*(sendBv+(i/8)) |=  mask(i%8);
							}else{
								if((*((short int*)(iMsg.data+wp))) != sysinfo->port){
									//send buffer to that port & add to record table
									sender = getRecordBySid(res.output);
			    						sysinfo->recordTable[sender].sid=res.output;
                            						sysinfo->recordTable[sender].port = (*((short int*)(iMsg.data+wp)));
			    						sysinfo->recordTable[sender].buffer = DEFAULT_BUFFER;
			    						sysinfo->recordTable[sender].status = 0;
                            						sysinfo->recordTable[sender].numTicks = DEFAULT_TICKS;
        								narad(SYS_MSG|BUFF_MSG,DEFAULT_STATUS,sender,bres.output,(bres.numByte+sizeof(int)),generatemsgid());
									pf((logs,"Sent Buffer to: %d\n",sysinfo->recordTable[sender].port));
								}
							}
							wp += sizeof(short int);
						}
						printRecordTable();
						sender = getRecordByPort(iMsg.from);
						if(doesExistMsgId(iMsg.messageId,SYS_MSG|PEER_MSG)){
							deleteByMsgId(iMsg.messageId);
                        				narad(SYS_MSG|OK,0,sender,"\0",1,iMsg.messageId);
		    	    				pf((logs,"Sent OK for Msg %d To: %d\n",iMsg.messageId,sysinfo->recordTable[sender].port));
						}else{
							//send set difference
							i=0;wp=0;
							while(i<tsize) {
								if((*(sendBv+(i/8))&mask(i%8))==0){
									res = encoder(sysinfo->recordTable[i].sid,strlen(sysinfo->recordTable[i].sid));
									j = 0;
									while(j < res.numByte){
										*(dat+wp+j) = res.output[j];
										j += 1;
									}
									wp += res.numByte;
									*((short int*)(dat+wp)) = sysinfo->recordTable[i].port;
									wp += sizeof(short int);
								}
							i++;
							}
							flg = 0;
                  					narad(SYS_MSG|PEER_MSG,DEFAULT_STATUS,sender,dat,wp,iMsg.messageId);
		    	    				pf((logs,"Sent Reply Peer Info to: %d\n",sysinfo->recordTable[sender].port));
							free(sendBv);
						}
						break;
					default:
		    				pf((logs,"Not a valid TYpe\n"));
				
				}	
			}
		}else{
			if(LRG_MSG == (iMsg.type&LRG_MSG)){
				// check if message with message id exists 
				// if yes then write to the file present in rrec->data
				// else whatever is in data treat it as a filename and respond with ok with same messageID 
				// |fname|dsize|noc|
				recieverRecord* rec = rdoesExistByMsgId(iMsg.messageId,iMsg.type);
				sender = getRecordByPort(iMsg.from);
				cs = iMsg.size - 2; 
				int timp=(sysinfo->recordTable[sender].buffer-(16+ceil(((strlen(sysinfo->systemId)*9)+1)/8.0)));
				if(NULL != rec){
					config = fopen((const char*)rec->data,"rb+");
					i = bitCountToIndex(*(iMsg.data+1),rec->bv,rec->bvc);
					*(rec->bv + ((int)i / 8)) |= mask((int)i % 8);
					j = 0;
					fseek(config,timp*i,SEEK_SET);
					fwrite(iMsg.data+2,1,cs,config);
					fclose(config);
				}else{
					res = decoder(iMsg.data);
					config = fopen((const char*)res.output,"rb+");
					fseek(config,*((int*)(iMsg.data+res.numByte)),SEEK_SET);
					fclose(config);
					addToRecieverTable(iMsg.type,DEFAULT_STATUS,iMsg.from,DEFAULT_TICKS,iMsg.messageId,res.output,*(iMsg.data+res.numByte+sizeof(int)),*((int*)(iMsg.data+res.numByte)));
					*((char*)&cs) = 6;
					narad(SYS_MSG|OK,0,sender,((char*)&cs),1,iMsg.messageId);
				}
				prt();
			}else{
				//Add to reciever Table
				pf((logs,"Received user short msg : %s\n",iMsg.data));
				sender = getRecordByPort(iMsg.from);
				narad(SYS_MSG|OK,0,sender,"\0",1,iMsg.messageId);
			}			  	
		}
      }else{}
        rcheckStateAndProcess();
	processCompleted();
        checkStateAndProcess();
      }
    }
  } else {
    pf((logs,"\nToo few arguments brain \n"));
  }
  fclose(logs);
  return 0;
}

void clear(char *str) {
	i=0;
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

int writeMetaData(unsigned char type, short int from, nRecord* nr, result messageId, result res){
	int offset=0,i;
	clm(bf);
	*((short int *)bf) = nr->port;
        offset += sizeof(short int);
        offset += sizeof(int);
        *(bf + offset) = type;
        offset += sizeof(unsigned char);
        *((short int *)(bf + offset)) = nr->port;
        offset += sizeof(short int);
        *((short int *)(bf + offset)) = from;
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
	return offset;
}

void dataWriter(int offset, char *data, int dsize){
	i = 0;
	while (i < dsize) {
		*(bf + offset + i) = data[i];
		i += 1;
	}
}

void narad(unsigned char type,char status,int index, unsigned char *data,int dsize,int mId) {
    int rp,offset = 0, i = 0, size,cs=0,rem;
    char  noc=1,ic=0;
    result res,messageId;
    messageId = encoder((unsigned char*)&mId,sizeof(int));
    nRecord* nr = &sysinfo->recordTable[index];
    res = encoder(nr->sid, strlen(nr->sid));
    size = sizeof(int) + sizeof(short int) * 2 + res.numByte + sizeof(char)+messageId.numByte;
    clm(bf);
    if (nr->buffer < size+dsize)
	type |= LRG_MSG;
    offset = writeMetaData(type,sysinfo->port,nr,messageId,res);
    if(SYS_MSG == (type&SYS_MSG)){
	if(LRG_MSG == (type&LRG_MSG)){
		// 1 for type
		cs = nr->buffer - (16 + res.numByte);	
		noc = ceil(dsize/(double)cs);
		*(bf+offset) = noc;
		offset += 1;
		ic=0;
		while(ic < noc){
	 		clm(bf+offset);
	 		*(bf+offset) = ic;
	 		if(ic == (noc-1)){
				if(dsize%cs==0)rem=cs;
				else rem=dsize%cs;
    				*((int *)(bf + sizeof(short int))) = (size+rem+2);
				dataWriter(offset+1,(data+(((int)ic)*cs)),rem);
			}else{ 
    				*((int *)(bf + sizeof(short int))) = (size+cs+2);
				dataWriter(offset+1,(data+(((int)ic)*cs)),cs);
			}		
			ic += 1;
    			sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int));
		}
	}else{
		size += dsize;
    		*((int *)(bf + sizeof(short int))) = size;
		dataWriter(offset,data,dsize);
    		sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int));
    	}
        if(!doesExistMsgId(mId,type)){
     		addToSenderTable(type,status,index,((nr->numTicks)*noc),mId,data,noc,dsize);
        }else{
 		senderRecord* temp = getRecordByMsgId(mId);
 		temp->status=DEFAULT_STATUS;
 		temp->numTicks=noc*nr->numTicks;
        }
	pst();
	}else{
    }
}

void printdecon(deconSys ds){
	pf((logs,"\nMID : %d\n",ds.messageId));
	pf((logs,"type : %d\n",ds.type));
	pf((logs,"size : %d\n",ds.size));
	pf((logs,"from : %d\n",ds.from));
	pf((logs,"to : %d\n",ds.to));
	pf((logs,"sysid : %s\n",ds.sysId));
	acces(ds.data,6);
// 	pf((logs,"bv : %b\n",*(ds.data)));
//	pf((logs,"data : %s\n",res.output));
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
    	offset += sizeof(unsigned char);
	output.size -= sizeof(char);
	output.to = *((short int *)(buffer + offset)) ;
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
	i =0;
	result res;
  	deconSys imsg;
	res = encoder(sysinfo->systemId,strlen(sysinfo->systemId));
	res.output = (unsigned char*)realloc(res.output,(res.numByte+sizeof(int)));
	*((int*)(res.output+res.numByte)) = sysinfo->sysBuffer;
	while(i < sysinfo->numRecords){
        narad(SYS_MSG|BUFF_MSG,DEFAULT_STATUS,i,res.output,(res.numByte+sizeof(int)),generatemsgid());
		i += 1;
	}

}
deconSys getFromEar(){
  int dsize,i;
  deconSys output;
  output.type = NOP;
  fseek(be, 0, SEEK_END);
  enew = ftell(be);
  dsize = enew - eold;
  if (dsize > 0 && 0 == (dsize % sysinfo->sysBuffer)) {
    if((dsize/sysinfo->sysBuffer) > 1) dsize = sysinfo->sysBuffer;
    fseek(be, eold, SEEK_SET);
    clm(bf);
    fread(bf,sysinfo->sysBuffer, 1, be);
    output =(convertSysMessage(bf));
    eold += sysinfo->sysBuffer;
  }
  return output;
}
//  void breakMsg(nRecord target, char* data, int dsize, char* bf){
//  if(target.buffer > dsize){
//        narad(2, sysinfo->port, target.port, target.sid, data, dsize, bf);
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
		 if(t->next==senderTable)pointer=prev;
		 // free the space
	 }
	 prev = t;
	 t = t->next;
 }

}
void rdeleteByMsgId(int messageId){
  	recieverRecord *rprev,*t;
	t = recieverTable;
 	rprev = t;
 	t = t->next;
	while(t != recieverTable){
	 if(t->messageId == messageId){
		 rprev->next = t->next;
		 if(t->next==recieverTable)rpointer=rprev;
		 // free the space
	 }
	 rprev = t;
	 t = t->next;
 }

}
senderRecord* getRecordByMsgId(int messageId){
  	senderRecord *prev,*t,*output=NULL;
	t = senderTable;
 	prev = t;
 	t = t->next;
	while(t != senderTable && output==NULL){
	 if(t->messageId == messageId){
		 output = t;
	 }
	 prev = t;
	 t = t->next;
 	}
	return output;
}
int searchRecordByPort(int port){
  int output = -1;
  i = 0;
  while(i<sysinfo->numRecords && output == -1){
	if(sysinfo->recordTable[i].port == port){
		output = i;
	}
	i += 1;
  }
  return output;
}
char doesExistMsgId(int messageId,unsigned char type){
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

recieverRecord* rdoesExistByMsgId(int messageId,unsigned char type) {
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
  i = 0;
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
  i = 0;
  while(i < sysinfo->numRecords){
    if(!strcmp((sysinfo->recordTable[i]).sid ,sid)){
      return i;
    }
i+=1;
  }

  return createRecord();
}
char doesExistbyTo(short int from,unsigned char type){
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
/***************************************
 * assuming numticks = noc*numticks
 * check numticks if 0
 * no then continue
 * yes then check bit vector all 1
 * if yes then assemble() and delete record & send "OK"
 * no then check status
 * if 0 then delete record & may be mark down
 * if non-zero then decrement status, send retry msg with bit vector, set timer(count(0,bv)*numticks)
 ***************************************/
void processCompleted(){
  	recieverRecord* rprev,*rt;
  	rt = recieverTable;
  	rprev = rt;
  	rt = rt->next;
 	char c;
	int sender=0;
  	while(rt != recieverTable){
  			// free the space in both if else
		if(rt->status == (DEFAULT_STATUS+1)){
			if(SYS_MSG == rt->type&SYS_MSG){
			switch(rt->type&B_CLEAR){
				case PEER_MSG:
		        		pf((logs,"\nCase 7: Recieved Large Peer Info From: %d\n",*((short int*)(rt->data+7))));
					wp = 0;
					bres = encoder(sysinfo->systemId,strlen(sysinfo->systemId));
					bres.output = (unsigned char*)realloc(bres.output,(bres.numByte+sizeof(int)));
					*((int*)(bres.output+bres.numByte)) = sysinfo->sysBuffer;
					i = 0;
					tsize  = 0;
					while(i < sysinfo->numRecords){
						tsize += ceil(((strlen(sysinfo->recordTable[i].sid)*9)+1)/8.0);
						i += 1;
					}
					i = 0;
					dat = (unsigned char*)malloc(tsize*sizeof(unsigned char)+sysinfo->numRecords*sizeof(short int));
					sendBvc=ceil(sysinfo->numRecords/8.0);
					sendBv=(unsigned char*)calloc(sendBvc,sizeof(unsigned char));
					tsize = sysinfo->numRecords;
					while(wp < rt->dsize){
						res = decoder(rt->data+wp);
						wp += res.numByte;
						i=searchRecordByPort(*((short int*)(rt->data+wp)));
						if(i!=-1){
							*(sendBv+(i/8)) |=  mask(i%8);
						}else{
							if((*((short int*)(rt->data+wp))) != sysinfo->port){
								//send buffer to that port & add to record table
								sender = getRecordBySid(res.output);
			    					sysinfo->recordTable[sender].sid=res.output;
                            					sysinfo->recordTable[sender].port = (*((short int*)(rt->data+wp)));
			    					sysinfo->recordTable[sender].buffer = DEFAULT_BUFFER;
			    					sysinfo->recordTable[sender].status = 0;
                            					sysinfo->recordTable[sender].numTicks = DEFAULT_TICKS;
        							narad(SYS_MSG|BUFF_MSG,DEFAULT_STATUS,sender,bres.output,(bres.numByte+sizeof(int)),generatemsgid());
								pf((logs,"Sent Buffer to: %d\n",sysinfo->recordTable[sender].port));
							}
						}
						wp += sizeof(short int);
					}
					sender = getRecordByPort(rt->from);
					if(doesExistMsgId(rt->messageId,SYS_MSG|LRG_MSG|PEER_MSG)){
						deleteByMsgId(rt->messageId);
                        			narad(SYS_MSG|OK,0,sender,"\0",1,rt->messageId);
		    	    			pf((logs,"Sent OK for Msg %d To: %d\n",rt->messageId,sysinfo->recordTable[sender].port));
					}else{
						//send set difference
						i=0;wp=0;
						while(i<tsize) {
							if((*(sendBv+(i/8))&mask(i%8))==0){
								res = encoder(sysinfo->recordTable[i].sid,strlen(sysinfo->recordTable[i].sid));
								j = 0;
								while(j < res.numByte){
									*(dat+wp+j) = res.output[j];
									j += 1;
								}
								wp += res.numByte;
								*((short int*)(dat+wp)) = sysinfo->recordTable[i].port;
								wp += sizeof(short int);
							}
						i++;
						}
						flg = 0;
                  				narad(SYS_MSG|PEER_MSG,DEFAULT_STATUS,sender,dat,wp,rt->messageId);
		    	    			pf((logs,"Sent Reply Peer Info to: %d\n",sysinfo->recordTable[sender].port));
						free(sendBv);
					}
					break;
				case 0x04:
					pf((logs,"%s\n",rt->data));
					break;
			}

			rprev->next = rt->next;
		 	if(rt->next==recieverTable)rpointer=rprev;
			}else{}
		
		}
  	  		rprev = rt;
  	  		rt = rt->next;
  	}
}
void rcheckStateAndProcess(){

  	recieverRecord* rprev,*rt;
  	rt = recieverTable;
  	rprev = rt;
  	rt = rt->next;
 	char c;
  	while(rt != recieverTable){
  			// free the space in both if else
  		  if((0 == rt->numTicks) && (0 == rt->status)&& ((DEFAULT_STATUS+1)!= rt->status)){
  			// TODO : mark the target system dead
  	 		rprev->next = rt->next;
		 	if(rt->next==recieverTable)rpointer=rprev;
  	 	   }else if((0==rt->numTicks)){
 			if((c=bitVectorContainsZero(rt->bv,rt->bvc))!=0){
          			narad((rt->type)+1,DEFAULT_STATUS,getRecordByPort(rt->from),rt->bv,ceil(rt->bvc/8.0),rt->messageId);
				pf((logs,"\nSent Resend request for Msg %d\n",rt->messageId));
 				rt->numTicks = (5*(c+1)*DEFAULT_TICKS);
 				rt->status--;
 			}else{
 				//assemble();
 				pf((logs,"\nRecieved whole Large Msg: %d\n",rt->messageId));
				rt->status = DEFAULT_STATUS + 1;
          			narad(OK,DEFAULT_STATUS,getRecordByPort(rt->from),"OK",2,rt->messageId);
 				pf((logs,"\nSent OK for Msg %d\n",rt->messageId));
				deleteByMsgId(rt->messageId);
 			}
               	}
  	  		rprev = rt;
  	  		rt = rt->next;
  	}
}
void checkStateAndProcess(){
  	senderRecord* prev,*t,*p;
  	t = senderTable;
  	prev = t;
  	t = t->next;
  	while(t != senderTable){
  		// free the space in both if else
  		//if(5 != t->type){
  		 if((0 == t->numTicks) && (0 == t->status)){
  			// mark the target system dead
  	 		prev->next = t->next;
 		 	if(t->next==senderTable)pointer=prev;
  	 	   }else if((0==t->numTicks)){
          		narad(t->type,(t->status-1),t->nr,t->message,t->dsize,t->messageId);
			pf((logs,"\nResent Msg %d to %d\n",t->messageId,sysinfo->recordTable[t->nr].port));
  	 		prev->next = t->next;
 		 	if(t->next==senderTable)pointer=prev;
              	   }
  		//}
  	  	prev = t;
  	  	t = t->next;
      	}
}
char bitVectorContainsZero(unsigned char* bv, char bvc){
	char i = 0,output=0;
	while(i < bvc){
		if((bv[i/8]&mask(i%8)) == 0) output++;
		i+=1;
	}
	return output;
}
char bitCountToIndex(char index,unsigned char* bv, char bvc){
	char i = 0,output=0,counter = 0;
	flg=0;
	while(i < bvc &&  counter <= index){
		if((bv[((int)i)/8] & mask(((int)i)%8)) == 0){
			if(i == index){
				output  = i;
				flg = 1;
			}else{
				counter += 1;
			}
		}else{

		}
		i+=1;
	}
	if(0 == flg){
		output = i-1;
	}
	return output;
}
void acces(unsigned char *out,int numbt) {
  int reqbits = numbt;
  i = 0;
  int bitsDone = 0;
//   pf((logs,"\n"));
  while (i < reqbits) {
    if ((*(out + (i / 8)) & (mask(i % 8))) != 0)
     { pf((logs,"1"));}
    else
     { pf((logs,"0"));}
    i += 1;
    if (i % 8 == 0){
		pf((logs,"  "))
	}
    else{
	pf((logs,""))
	}
  }
  pf((logs,"\n"));
}

