#define DEFAULT_BUFFER 512
#define addToSenderTable(a,b,c,d,e,f) {  temp = pointer->next; \
	pointer->next = createSenderRecord(a,b,c,d,e,f);\
	pointer->next->next = temp;\
	pointer = pointer->next;\
	} 
typedef struct {
  char *sid;
  short int port;
  int buffer;
  char status;
 long numTicks;	
} nRecord;

typedef struct{
 char type;
 short int from;
 short int to;
 unsigned char *sysId;
 unsigned char *data;
}deconSys;

struct senderR{
 char type;
 char status;
 nRecord *nr;
 long numTicks;	
 result messageId;
 char* message;
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

senderRecord* createSenderRecord(char type,char status,nRecord* nr,long numTicks,result messageId,char* message);
