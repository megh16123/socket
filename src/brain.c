#include"ecdc.h"
#include"brainutil.h"
#include <time.h>
#define pf(a) logs = fopen(id,"a"); fprintf a ; fclose(logs);
#define DEBUG pf((logs,"\nMesg Came\n"));
#define SYS_MSG 0x80 
#define LRG_MSG	0x40
#define BUFF_MSG 0x01
#define BUFF_REPLY_MSG 0x02
#define PEER_MSG 0x03
#define RESEND 0x04
#define OK 0x00
#define B_CLEAR 0x3F
#define NOP 255
#define NOC_IC 2*sizeof(unsigned int)
#define IC sizeof(unsigned int)
#define UINT(x) ((unsigned int*)(x))

static long sysTime = 0;
sysInfo *sysinfo;
senderRecord *senderTable,*temp,*pointer;
recieverRecord *recieverTable,*rtemp,*rpointer;
int numRecords=0,tsize=0,enew=0, eold=0,unew,uold,sendBvc = 0,j,wp;
unsigned char timeFlag=1,flg=0,*bf,*id,*intfiles[4],*sendBv,*dat;
FILE *f, *be, *uib,*bui, *logs,*config;
result res,bres;
unsigned int i;

void sendToFile(unsigned char *fname, unsigned char *buf, int size) {
unsigned char ch; 
  if(strcmp(fname,intfiles[1])){     
  	f = fopen(fname, "ab");
  	fwrite(buf, sizeof(unsigned char), size, f);
  	fclose(f);
  }
  else{
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
	fwrite(buf,sizeof(unsigned char),size,f);
	fseek(f,0,SEEK_SET);
	fputc(ch,f);
	fclose(f);
	
  }
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
    usleep(400);
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
   while(temp != senderTable){
	if((SYS_MSG&temp->type) == SYS_MSG){
          pf((logs,"  %d\t\t%d\t%s\n",temp->messageId,temp->type,sysinfo->recordTable[temp->nr].sid));
	}else{
          pf((logs,"  %d\t\t%d\t%s\t%s\n",temp->messageId,temp->type,sysinfo->recordTable[temp->nr].sid,temp->message));
	}
 	  prev = temp;
 	  temp = temp->next;
}
   pf((logs,"---------------------------------\n"));
}

int main(int argc, char **argv){
    time_t t,t1,t2;
    srand((unsigned)time(&t));
    deconSys iMsg;
    if (argc == 2){
    	config = fopen(argv[1], "r");
    	int linesRead = 0;
    	i = 0; j = 0;
    	int recordIt = 0,cs;
    	char* dat,*fname;
    	char *line;
    	pthread_t tmp;
    	sysinfo = (sysInfo *)malloc(sizeof(sysInfo));
    	if (NULL == config) {
      		pf((logs,"Config File didn't open \n"));
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
		printRecordTable();

		free(line);
	pf((logs,"Configs File Read\n"));
	senderTable = createSenderRecord(0,0,-1,0,-1,NULL,0,0);
	recieverTable = createRecieverRecord(0,0,-1,0,-1,NULL,0,0);
	senderTable->next = senderTable;
	recieverTable->next = recieverTable;
	pointer = senderTable;
	rpointer = recieverTable;
        pthread_create(&tmp, NULL, (void *)&sys_tick, NULL);
        pthread_detach(tmp);
        bf = (unsigned char*)malloc(sysinfo->sysBuffer + sizeof(short int)+IC);
        clm(bf);
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
				recieverRecord* rec = rdoesExistByMsgId(iMsg.messageId,iMsg.type,iMsg.from);
				sender = getRecordByPort(iMsg.from);
				cs = iMsg.size - NOC_IC; 
				int timp=(sysinfo->recordTable[sender].buffer-(14+NOC_IC+ceil(((strlen(sysinfo->systemId)*9)+1)/8.0)));
				if(NULL != rec){
					rec->dsize += cs;
					i = bitCountToIndex(*UINT(iMsg.data+IC),rec->bv,rec->bvc);
					*(rec->bv + (i / 8)) |= mask(i % 8);
					j = 0;
					while(j < cs){
						*(rec->data + timp*(i)+j) = *(iMsg.data+NOC_IC+j);
						j+=1;
					}
				}else{
					addToRecieverTable(iMsg.type,DEFAULT_STATUS,iMsg.from,DEFAULT_TICKS,iMsg.messageId,(char*)malloc(timp*(*UINT(iMsg.data))),*UINT(iMsg.data),cs);
					i = 0;
					while(i < cs){
						*(rpointer->data + cs*(*(iMsg.data+IC))+i) = *(iMsg.data+NOC_IC+i);
						i+=1;
					}
					i = 0;
					*(rpointer->bv + (((*UINT(iMsg.data+IC))) / 8)) |= mask((((*UINT(iMsg.data+IC))) % 8));
				}
			}else{
				switch(iMsg.type&B_CLEAR){
				
					case OK:
		  				pf((logs,"Case 0: Recieved OK for Message: %d From: %d\n",iMsg.messageId,iMsg.from));
						if(7 == *(iMsg.data)){
                  					sender = getRecordByPort(iMsg.from);
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
               	  					deleteByMsgId(iMsg.messageId);
		  				}else if(6 == *(iMsg.data)){
							senderRecord *srec = getRecordByMsgId(iMsg.messageId);
							srec->status = DEFAULT_STATUS+1;
							srec->numTicks = 0;
							
						}else if(3 == *(iMsg.data)){
    							bui = fopen(intfiles[3],"rb+");
							fseek(bui,sizeof(int),SEEK_SET);
							fprintf(bui,"\b\b\bMessage sent\n");
							fseek(bui,0,SEEK_SET);
							fputc(0x11,bui);
							fputc(0x00,bui);
							fputc(0x00,bui);
							fputc(0x00,bui);
							fclose(bui);
               	  					deleteByMsgId(iMsg.messageId);
						}else{
               	  					deleteByMsgId(iMsg.messageId);
						}

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
		    				pf((logs,"1:Sent OK for Msg: %d To: %d\n",iMsg.messageId,iMsg.from));
                    				}
                    				break;
					case PEER_MSG:
		        			pf((logs,"\nCase 7: Recieved Large Peer Info From: %d\n",iMsg.from));
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
		    	    				pf((logs,"2:Sent OK for Msg %d To: %d\n",iMsg.messageId,sysinfo->recordTable[sender].port));
						}else{
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
                  					narad(SYS_MSG|PEER_MSG,DEFAULT_STATUS,sender,dat,wp,iMsg.messageId);
		    	    				pf((logs,"Sent Reply Peer Info to: %d\n",sysinfo->recordTable[sender].port));
							free(sendBv);
						}
							flg = 0;
						break;
					
					case RESEND:

		    				pf((logs,"\nCase 4: Recieved resend request from: %d\n",iMsg.from));
						senderRecord *srec = getRecordByMsgId(iMsg.messageId);
						memcpy(srec->bv,iMsg.data,ceil(srec->bvc/8.0));
						srec->status = DEFAULT_STATUS+1;
						srec->numTicks = 0;
						break;
					default:
		    				pf((logs,"Not a valid TYpe\n"));
				
				}	
			}
		}else{
			if(LRG_MSG == (iMsg.type&LRG_MSG)){
				recieverRecord* rec = rdoesExistByMsgId(iMsg.messageId,iMsg.type,iMsg.from);
				sender = getRecordByPort(iMsg.from);
				cs = iMsg.size-IC; 
				int timp = (sysinfo->recordTable[sender].buffer-(5+IC+sizeof(short int)+sizeof(int)));
				if(NULL != rec){
					config = fopen((const char*)rec->data,"rb+");
					i = *UINT(iMsg.data);
					*(rec->bv + (i / 8)) |= mask(i % 8);
					fseek(config,timp*i,SEEK_SET);
					fwrite(iMsg.data+IC,sizeof(unsigned char),cs,config);
					fclose(config);
					rec->numTicks = DEFAULT_TICKS;
					rec->status = DEFAULT_STATUS;
				}else{
					res = decoder(iMsg.data);
					res.output = (unsigned char*)realloc(res.output,res.numByte+ceil(log10(iMsg.messageId))+2);
					sprintf(res.output,"%s_%d",res.output,iMsg.messageId);
					config = fopen((const char*)res.output,"wb+");
					fseek(config,*((int*)(iMsg.data+res.numByte)),SEEK_SET);
					fclose(config);
					addToRecieverTable(iMsg.type,DEFAULT_STATUS,iMsg.from,*UINT(iMsg.data+res.numByte+sizeof(int))*DEFAULT_TICKS,iMsg.messageId,res.output,*UINT(iMsg.data+res.numByte+sizeof(int)),*((int*)(iMsg.data+res.numByte)));
					*((unsigned char*)&cs) = 6;
					narad(SYS_MSG|OK,0,sender,((unsigned char*)&cs),1,iMsg.messageId);
		    	    		pf((logs,"8:Sent OK for Msg %d To: %d\n",iMsg.messageId,sysinfo->recordTable[sender].port));
				}
			}else{
				pf((logs,"Received user short msg type: %d from %d\n",iMsg.type,iMsg.from));
				printdecon(iMsg);
				sender = getRecordByPort(iMsg.from);
				narad(SYS_MSG|OK,0,sender,"\0",1,iMsg.messageId);
		    	    	pf((logs,"9:Sent OK for Msg %d To: %d\n",iMsg.messageId,sysinfo->recordTable[sender].port));
			}			  	
		}
      }
	processUI();
        rcheckStateAndProcess();
	processFiles();
	processCompleted();
        checkStateAndProcess();
      }
    }
  } else {
    pf((logs,"\nToo few arguments brain \n"));
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
	*((int*)(bf+offset))=nr->buffer;
   	offset += sizeof(int);
        offset += sizeof(int);
        i = 0;
        while(i < messageId.numByte){
          *(bf + offset + i) = messageId.output[i];
          i += 1;
        }
      	offset += messageId.numByte;
        *((short int *)(bf + offset)) = from;
        offset += sizeof(short int);
        *(bf + offset) = type;
        offset += sizeof(unsigned char);
        *((short int *)(bf + offset)) = nr->port;
        offset += sizeof(short int);
      	i = 0;
      	while (i < res.numByte) {
        	*(bf + offset + i) = res.output[i];
        	i += 1;
      	}
      	offset += res.numByte;
	return offset;
}

void dataWriter(int offset, unsigned char *data, int dsize){
	i = 0;
	while (i < dsize) {
		*(bf + offset + i) = data[i];
		i += 1;
	}
}

void narad(unsigned char type,char status,int index, unsigned char *data,int dsize,int mId) {
    int rp,offset = 0, i = 0, size,cs=0,rem;
    unsigned int  noc=1,ic=0;
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
		cs = nr->buffer - (14+NOC_IC + res.numByte);	
		noc = ceil(dsize/(double)cs);
		*UINT(bf+offset) = noc;
		offset += IC;
		ic=0;
		while(ic < noc){
	 		clm(bf+offset);
	 		*UINT(bf+offset) = ic;
	 		if(ic == (noc-1)){
				if(dsize%cs==0)rem=cs;
				else rem=dsize%cs;
    				*((int *)(bf + sizeof(short int)+IC)) = (size+rem+NOC_IC);
				dataWriter(offset+IC,(data+((ic)*cs)),rem);
			}else{ 
    				*((int *)(bf + sizeof(short int)+IC)) = (size+cs+NOC_IC);
				dataWriter(offset+IC,(data+((ic)*cs)),cs);
			}		
			ic += 1;
    			sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int)+IC);
		}
	}else{
		size += dsize;
    		*((int *)(bf + sizeof(short int)+IC)) = size;
		dataWriter(offset,data,dsize);
    		sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int)+IC);
    	}
        if(!doesExistMsgId(mId,type)){
     		addToSenderTable(type,status,index,((nr->numTicks)*noc),mId,data,noc,dsize);
        }else{
 		senderRecord* temp = getRecordByMsgId(mId);
 		temp->status=status;
 		temp->numTicks=noc*nr->numTicks;
        }
	}else{
	if(LRG_MSG == (type&LRG_MSG)){
		cs = nr->buffer - (5+IC+sizeof(short int)+sizeof(int));
		noc = ceil(dsize/(double)cs);
		res = encoder(data,strlen(data)+1);
    		*((int *)(bf + sizeof(short int)+IC)) = (size+res.numByte+sizeof(int)+IC);
		dataWriter(offset,res.output,res.numByte);
		offset += res.numByte;
		*((int*)(bf+offset)) = dsize;
		offset += sizeof(int);
		*UINT(bf+offset) = noc;
		sendToFile(intfiles[1],bf,sysinfo->sysBuffer+sizeof(short int)+IC);
     		addToSenderTable(type,status,index,((nr->numTicks)*noc),mId,data,noc,dsize);
	}else{}	
    }
	pst();

}

void printdecon(deconSys ds){
	pf((logs,"\nMID : %d\n",ds.messageId));
	pf((logs,"type : %d\n",ds.type));
	pf((logs,"size : %d\n",ds.size));
	pf((logs,"from : %d\n",ds.from));
}
deconSys convertSysMessage(unsigned char *buffer){
deconSys output;
result res;
recieverRecord *rrec;
int offset = 0,i=0,dsize=0;
output.size = *((int*)buffer);
if(*((int*)buffer) <= sysinfo->sysBuffer){
	offset += sizeof(int);
	output.size -= sizeof(int);
        res = decoder((unsigned char*)(buffer+offset));
        output.messageId = *((int*)res.output);
        offset += res.numByte;
	output.size -= res.numByte;
	output.from = *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
	output.size -= sizeof(short int);
	rrec = rdoesExistByMsgId(output.messageId,LRG_MSG,output.from);
	if(NULL == rrec){
	output.type = *(buffer + offset) ;
    	offset += sizeof(unsigned char);
	output.size -= sizeof(char);
	output.to = *((short int *)(buffer + offset)) ;
    	offset += sizeof(short int);
	output.size -= sizeof(short int);
	res = decoder((unsigned char*)(buffer+offset));
	output.sysId = res.output;
	offset += res.numByte;
	output.size -= res.numByte;
	dsize = (*((int*)buffer)-offset);
	output.data = (unsigned char*)malloc(dsize);
	i = 0;
	while(i < dsize){
		output.data[i] = *(buffer+offset+i);
		i += 1;
	}
	}else{
		output.type = LRG_MSG;
		dsize = (*((int*)buffer)-offset);
		output.data = (unsigned char*)malloc(dsize);
		output.from = rrec->from;
		i = 0;
		while(i < dsize){
			output.data[i] = *(buffer+offset+i);
			i += 1;
		}
	}
}
return output;
}
void bufferExchng() {
	int bi;
	bi =0;
	result res;
  	deconSys imsg;
	res = encoder(sysinfo->systemId,strlen(sysinfo->systemId));
	res.output = (unsigned char*)realloc(res.output,(res.numByte+sizeof(int)));
	*((int*)(res.output+res.numByte)) = sysinfo->sysBuffer;
	while(bi < sysinfo->numRecords){
        narad(SYS_MSG|BUFF_MSG,DEFAULT_STATUS,bi,res.output,(res.numByte+sizeof(int)),generatemsgid());
		pf((logs,"NR %d: %d\n",bi,sysinfo->numRecords));
		bi += 1;
	}

}

unsigned char processUI(){
  uib = fopen(intfiles[2],"rb+");
  unsigned char ubf[sysinfo->sysBuffer];
  unsigned char type = 1,output=1,ch;
  int offset = 0,k;
  fseek(uib,0,SEEK_SET);
  ch = fgetc(uib);
  if(ch == '0'){
   	clm(ubf);
   	ch = '1';
   	fseek(uib,1,SEEK_SET);
    	fread(ubf,sizeof(unsigned char),sysinfo->sysBuffer, uib);
   	fseek(uib,0,SEEK_SET);
	fwrite(&ch,1,1,uib);
    	type = *ubf;
    	bui = fopen(intfiles[3],"rb+");
    	fseek(bui,0,SEEK_SET);
    	switch(type){
    	case '1':
		output = 0;
 		narad(LRG_MSG,DEFAULT_STATUS,*((int*)(ubf+1)),strdup((unsigned char*)(ubf+(2*sizeof(int))+1)),*((int*)(ubf+sizeof(int)+1)),generatemsgid());
		break;
	case '2':	
		while(fread(&k,sizeof(int),1,bui)&& (k != -1)){
			fclose(bui);
			bui = fopen(intfiles[3],"rb+");
			fseek(bui,0,SEEK_SET);
		}

		fseek(bui,sizeof(int),SEEK_SET);	
		
		i = 0;
		recieverRecord *rt,*rprev;
 		rprev = rt;
  		rt = recieverTable;
  		rt = rt->next;
  		while(rt != recieverTable){
			if(rt->status == DEFAULT_STATUS+1){
				fprintf(bui,"%s: %s\n",sysinfo->recordTable[getRecordByPort(rt->from)].sid,rt->data);
				i++;
		 		rprev->next = rt->next;
		 		if(rt->next==recieverTable)rpointer=rprev;
  	  			rt = rt->next;
			}else{
			rprev = rt;
  	  		rt = rt->next;
			}
  		}

		fputc('\n',bui);
		k = ftell(bui)-sizeof(int);
		fseek(bui,0,SEEK_SET);
		fwrite(&k,sizeof(int),1,bui);
		fclose(bui);
		break;
	case '3':
		while(fread(&k,sizeof(int),1,bui)&& (k != -1)){
			fclose(bui);
			bui = fopen(intfiles[3],"rb+");
			fseek(bui,0,SEEK_SET);
		}

		k = 0;
		fseek(bui,sizeof(int),SEEK_SET);	
		i = 0;
		while(i < sysinfo->numRecords){
			fprintf(bui,"%s\n",sysinfo->recordTable[i++].sid);
		}
		fputc('\n',bui);
		k = ftell(bui)-sizeof(int);
		fseek(bui,0,SEEK_SET);
		fwrite(&k,sizeof(int),1,bui);
		fclose(bui);

		break;
    }
  }
 fclose(uib);
  return output;
}

deconSys getFromEar(){
  deconSys output;
  output.type = NOP;
  unsigned char ch;
  FILE *f = fopen(intfiles[0],"rb+");
   fseek(f,0,SEEK_SET);
   ch = fgetc(f);
   if(ch == '0'){
   	ch = '1';
   	fseek(f,1,SEEK_SET);
	fread(bf,sizeof(unsigned char),sysinfo->sysBuffer,f);
   	fseek(f,0,SEEK_SET);
	fwrite(&ch,1,1,f);
	output=convertSysMessage(bf);
   }
  fclose(f);

  return output;

}

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
	 if((t->messageId == messageId) && (t->type == type)){
	 output = 1;
	 }
	 prev = t;
	 t = t->next;
	}
    return output;
}

recieverRecord* rdoesExistByMsgId(int messageId,unsigned char type,short int from) {
  recieverRecord *output=NULL;
  recieverRecord* prev,*t;
  t = recieverTable;
  prev = t;
  t = t->next;
  while(t != recieverTable && output==NULL ){
	 if((t->messageId == messageId) && (t->type == type) && (from == t->from)){
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
void processCompleted(){
  	recieverRecord* rprev,*rt;
  	rt = recieverTable;
  	rprev = rt;
  	rt = rt->next;
 	unsigned char c;
	int sender=0;
  	while(rt != recieverTable){
  			// free the space in both if else
		if(rt->status == (DEFAULT_STATUS+1)){
			if(SYS_MSG == (rt->type&SYS_MSG)){
				switch(rt->type&B_CLEAR){
					case PEER_MSG:
						c = 200;
		          			narad(SYS_MSG|OK,0,getRecordByPort(rt->from),&c,1,rt->messageId);
		 				pf((logs,"\n3:Sent OK for Msg %d\n",rt->messageId));
						deleteByMsgId(rt->messageId);
			        		pf((logs,"\nCase 7: Recieved Large Peer Info From: %d\n",rt->from));
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
			    	    			pf((logs,"4:Sent OK for Msg %d To: %d\n",rt->messageId,sysinfo->recordTable[sender].port));
						}else{
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
					case RESEND:	
				        	senderRecord *srec = getRecordByMsgId(rt->messageId);
						memcpy(srec->bv,rt->data,ceil(srec->bvc/8.0));
						srec->status = DEFAULT_STATUS+1;
						srec->numTicks = 0;
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
 	unsigned int c1;
 	unsigned char c;
  	while(rt != recieverTable){
		if ((DEFAULT_STATUS+1)!= rt->status){
  			// free the space in both if else
  		  if((0 == rt->numTicks) && (0 == rt->status)){ 
  			// TODO : mark the target system dead
  	 		rprev->next = rt->next;
		 	if(rt->next==recieverTable)rpointer=rprev;
  	 	   }else if((0==rt->numTicks)){
 			if((c1=bitVectorContainsZero(rt->bv,rt->bvc))!=0){
				if((B_CLEAR&rt->type) != RESEND){
	          			narad(SYS_MSG|RESEND,0,getRecordByPort(rt->from),rt->bv,ceil(rt->bvc/8.0),rt->messageId);
					pf((logs,"\nSent Resend request for Msg %d\n",rt->messageId));
	 				rt->numTicks = (c1*DEFAULT_TICKS);
					deleteByMsgId(rt->messageId);
				}
 				rt->status--;
 			}else{
 				pf((logs,"\nRecieved whole Large Msg: %d\n",rt->messageId));
				rt->status = DEFAULT_STATUS + 1;
				if((SYS_MSG&rt->type) == SYS_MSG){
					c = 200;
				}else{
					c = 3;
					narad(SYS_MSG|OK,0,getRecordByPort(rt->from),&c,1,rt->messageId);
	 				pf((logs,"\n5:Sent OK for Msg %d\n",rt->messageId));
				}
 			}
               	    }
		  else{
 			if(bitVectorContainsZero(rt->bv,rt->bvc)==0){
 				pf((logs,"\nRecieved whole Large Msg: %d,type: %d\n",rt->messageId,rt->type));
				rt->status = DEFAULT_STATUS + 1;
				if((SYS_MSG&(rt->type)) == SYS_MSG){
				c = 200;
				}else{
					c = 3;
	          			narad(SYS_MSG|OK,0,getRecordByPort(rt->from),&c,1,rt->messageId);
	 				pf((logs,"\n6:Sent OK for Msg %d\n",rt->messageId));
				}
 			}


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
		if((DEFAULT_STATUS) >= t->status){
  		 if((0 == t->numTicks) && (0 == t->status)){
  			// mark the target system dead
			if((t->type&SYS_MSG) == SYS_MSG){
			}else{	
				bui = fopen(intfiles[3],"rb+");
				fseek(bui,sizeof(int),SEEK_SET);
				fprintf(bui,"\b\b\bCouldn't send Message\n");
				fseek(bui,0,SEEK_SET);
				fputc(0x1A,bui);
				fputc(0x00,bui);
				fputc(0x00,bui);
				fputc(0x00,bui);
				fclose(bui);
			}
  	 		prev->next = t->next;
 		 	if(t->next==senderTable)pointer=prev;
  	 	   }else if((0==t->numTicks)){
          		narad(t->type,(t->status-1),t->nr,t->message,t->dsize,t->messageId);
			pf((logs,"\nResent Msg %d to %d\n",t->messageId,sysinfo->recordTable[t->nr].port));
  	 		prev->next = t->next;
 		 	if(t->next==senderTable)pointer=prev;
              	   }
		}
  	  	prev = t;
  	  	t = t->next;
      	}
}

void processFiles(){
  	senderRecord* prev,*t,*p;
	unsigned int i=0,j=0;
	int cs,offset,size=(sizeof(short int)+sizeof(int)),rem;
  	t = senderTable;
  	prev = t;
  	t = t->next;
	FILE *f;
	result res;
  	while(t != senderTable){
  		// free the space in both if else
		if(((DEFAULT_STATUS+3) == t->status) && (t->numTicks == 0)){
			bui = fopen(intfiles[3],"rb+");
			fseek(bui,sizeof(int),SEEK_SET);
			fprintf(bui,"\b\b\bCouldn't send Message\n");
			fseek(bui,0,SEEK_SET);
			fputc(0x1A,bui);
			fputc(0x00,bui);
			fputc(0x00,bui);
			fputc(0x00,bui);
			fclose(bui);
  	 		prev->next = t->next;
 		 	if(t->next==senderTable)pointer=prev;
		}else if(((DEFAULT_STATUS+1) <= t->status) && (t->numTicks == 0)){
		
			i = 0;j=0;offset = 0;
			f = fopen((const char*)t->message,"rb");
			cs = sysinfo->recordTable[t->nr].buffer - (5+IC + sizeof(short int)+sizeof(int));
			clm(bf);
			*((short int*)bf) = sysinfo->recordTable[t->nr].port;
			offset += sizeof(short int);
			*((int*)(bf+offset))=sysinfo->recordTable[t->nr].buffer;
   			offset += sizeof(int);
			offset += sizeof(int);
			res = encoder((unsigned char*)&(t->messageId),sizeof(int));
			while(j < res.numByte){
				*(bf+offset+j) = res.output[j];
				j += 1;
			}
			offset += res.numByte;
			*((short int*)(bf+offset)) = sysinfo->port;
			offset += sizeof(short int);
			char flg=0;
			while(i < t->bvc){
			if((t->bv[i/8]&mask(i%8)) == 0){
				flg=1;
				*UINT(bf+offset)= i;
				clm(bf+offset+IC);
				if(i == t->bvc-1){
                                	if(t->dsize%cs==0)rem=cs;
                                	else rem=t->dsize%cs;
                                	*((int *)(bf + sizeof(short int)+IC)) = (size+rem+IC+res.numByte);
					fseek(f,i*cs,SEEK_SET);
					fread((bf+offset+IC),sizeof(unsigned char),rem,f);
                        	}else{ 
                                	*((int *)(bf + sizeof(short int)+IC)) = (size+cs+IC+res.numByte);
					fseek(f,i*cs,SEEK_SET);
					fread((bf+offset+IC),sizeof(unsigned char),cs,f);
				}   	
	    				sendToFile(intfiles[1], bf, sysinfo->sysBuffer + sizeof(short int)+IC);
			}
				i += 1;
			}
			if(flg==1)pf((logs,"\nResent Msg %d to %d\n",t->messageId,sysinfo->recordTable[t->nr].port));
			t->status++;
			t->numTicks = (t->bvc)*sysinfo->recordTable[t->nr].numTicks;
			fclose(f);
		}
  	  	prev = t;
  	  	t = t->next;
      	}
}


unsigned int bitVectorContainsZero(unsigned char* bv,unsigned int bvc){
	unsigned int i = 0,output=0;
	while(i < bvc){
		if((bv[i/8]&mask(i%8)) != 0){ 
		
		}else{
			output++;
		}
		i+=1;
	}
	return output;
}
unsigned int bitCountToIndex(unsigned int index,unsigned char* bv, unsigned int bvc){
	unsigned int i = 0,output=0,counter = 0;
	flg=0;
	while(i < bvc &&  counter <= index){
		if((bv[(i)/8] & mask((i)%8)) == 0){
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

