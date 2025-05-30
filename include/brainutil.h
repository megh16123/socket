#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<math.h>
#define DEFAULT_BUFFER 512 
#define DEFAULT_TICKS 5 
#define DEFAULT_STATUS 3 
#define addToSenderTable(a,b,c,d,e,f,g,h) {  temp = pointer->next; \
	pointer->next = createSenderRecord(a,b,c,d,e,f,g,h);\
	pointer->next->next = temp;\
	pointer = pointer->next;\
	} 

#define addToRecieverTable(a,b,c,d,e,f,g,h) {  rtemp = rpointer->next; \
	rpointer->next = createRecieverRecord(a,b,c,d,e,f,g,h);\
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
 unsigned char type;
 int size;
 short int from;
 short int to;
 int messageId;
 unsigned char *sysId;
 unsigned char *data;
}deconSys;

struct senderR{
 unsigned char type;
 char status;
 int nr;
 int dsize;
 long numTicks;	
 int messageId;
 unsigned char* message;
 unsigned int bvc;
 unsigned char* bv;
 struct senderR* next;
};

typedef struct senderR senderRecord;

struct recieverR{
 unsigned char type;
 char status;
 short int from;
 long numTicks;	
 int messageId;
 int dsize;
 unsigned char* data;
 unsigned int bvc;
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

senderRecord* createSenderRecord(unsigned char type,char status,int nr,long numTicks,int messageId,unsigned char* message,unsigned int bvc,int dsize);

recieverRecord* createRecieverRecord(unsigned char type,char status,short int from,long numTicks,int messageId,unsigned char* message,unsigned int bvc,int dsize);

void narad(unsigned char,char, int, unsigned char*, int,int);
void printdecon(deconSys);
deconSys convertSysMessage(unsigned char*);
void bufferExchng();
deconSys getFromEar();
int generatemsgid();
void deleteByMsgId(int messageId);
char doesExistMsgId(int messageId,unsigned char type);
recieverRecord* rdoesExistByMsgId(int messageId,unsigned char type,short int from);
char doesExistbyTo(short int from,unsigned char type);
int getRecordByPort(short int from);
int getRecordBySid(char* sid);
int createRecord();
void checkStateAndProcess();
unsigned int bitVectorContainsZero(unsigned char* bv,unsigned int bvc);
unsigned int bitCountToIndex(unsigned int index,unsigned char* bv, unsigned int bvc);
senderRecord* getRecordByMsgId(int messageId);
void rcheckStateAndProcess();
void acces(unsigned char *out,int numbt);
void processCompleted();
int searchRecordByPort(int);
void processFiles();
unsigned char processUI();
