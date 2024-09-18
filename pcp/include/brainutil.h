#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<math.h>
#define DEFAULT_BUFFER 512
#define DEFAULT_TICKS 1000
#define DEFAULT_STATUS 3 
#define addToSenderTable(a,b,c,d,e,f,g) {  temp = pointer->next; \
	pointer->next = createSenderRecord(a,b,c,d,e,f,g);\
	pointer->next->next = temp;\
	pointer = pointer->next;\
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
 long numTicks;	
 int messageId;
 char* message;
 int bvc;
 unsigned char* bv;
 struct senderR* next;
};

typedef struct senderR senderRecord;

typedef struct {
  unsigned char *systemId;
  short int port;
  int numRecords;
  nRecord *recordTable;
  int sysBuffer;
} sysInfo;

senderRecord* createSenderRecord(char type,char status,int nr,long numTicks,int messageId,char* message,int bvc);

void createSysMessage(char,char, int, unsigned char*, int,int);
void printdecon(deconSys);
deconSys convertSysMessage(char*);
void bufferExchng();
deconSys getFromEar();
int generateMsgId();
void deleteByMsgId(int messageId);
char doesExistMsgId(int messageId,char type);
char doesExistbyTo(short int from,char type);
int getRecordByPort(short int from);
int getRecordBySid(char* sid);
int createRecord();
void checkStateAndProcess();
