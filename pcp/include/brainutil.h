#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<math.h>
#define DEFAULT_BUFFER 512
#define DEFAULT_TICKS 1000
#define DEFAULT_STATUS 3 
#define addToSenderTable(a,b,c,d,e,f,g,h) {  temp = pointer->next; \
	pointer->next = createSenderRecord(a,b,c,d,e,f,g,h);\
	pointer->next->next = temp;\
	pointer = pointer->next;\
	} 

#define addToRecieverTable(a,b,c,d,e,f,g) {  rtemp = rpointer->next; \
	rpointer->next = createRecieverRecord(a,b,c,d,e,f,g);\
	rpointer->next->next = rtemp;\
	rpointer = rpointer->next;\
	} 

typedef struct {
  unsigned char *sid;
  short int port;
  int buffer;
  char status;
 long numTicks;	
} nRecord;

typedef struct{
 char type;
 int size;
 short int from;
 short int to;
 int messageId;
 unsigned char *sysId;
 unsigned char *data;
}deconSys;

struct senderR{
 char type;
 char status;
 int nr;
 int dsize;
 long numTicks;	
 int messageId;
 char* message;
 char bvc;
 unsigned char* bv;
 struct senderR* next;
};

typedef struct senderR senderRecord;

struct recieverR{
 char type;
 char status;
 short int from;
 long numTicks;	
 int messageId;
 char* data;
 char bvc;
 unsigned char* bv;
 struct recieverR* next;
};

typedef struct recieverR recieverRecord;


typedef struct {
  unsigned char *systemId;
  short int port;
  int numRecords;
  nRecord *recordTable;
  int sysBuffer;
} sysInfo;

senderRecord* createSenderRecord(char type,char status,int nr,long numTicks,int messageId,char* message,char bvc,int dsize);

recieverRecord* createRecieverRecord(char type,char status,short int from,long numTicks,int messageId,char* message,char bvc);

void createSysMessage(char,char, int, unsigned char*, int,int);
void printdecon(deconSys);
deconSys convertSysMessage(char*);
void bufferExchng();
deconSys getFromEar();
int generateMsgId();
void deleteByMsgId(int messageId);
char doesExistMsgId(int messageId,char type);
recieverRecord* rdoesExistByMsgId(int messageId,char type);
char doesExistbyTo(short int from,char type);
int getRecordByPort(short int from);
int getRecordBySid(char* sid);
int createRecord();
void checkStateAndProcess();
char bitVectorContainsZero(unsigned char* bv, char bvc);
char bitCountToIndex(char index,unsigned char* bv, char bvc);
senderRecord* getRecordByMsgId(int messageId);
