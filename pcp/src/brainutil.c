#include"ecdc.h"
#include"brainutil.h"
senderRecord* createSenderRecord(char type,char status,int nr,long numTicks,int messageId,char* message){
	senderRecord* output = (senderRecord*)malloc(sizeof(senderRecord));
	output->type = type;
	output->status = status;
	output->nr = nr;
	output->numTicks = numTicks;
	output->messageId = messageId;
	output->message = message;
	output->next = NULL;
	return output;
}
