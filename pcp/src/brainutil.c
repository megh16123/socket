#include"ecdc.h"
#include"brainutil.h"
senderRecord* createSenderRecord(char type,char status,int nr,long numTicks,int messageId,char* message,char bvc){
	senderRecord* output = (senderRecord*)malloc(sizeof(senderRecord));
	output->type = type;
	output->status = status;
	output->nr = nr;
	output->bvc = bvc;
	output->bv = (0==bvc)?NULL:(unsigned char*)calloc(ceil(bvc/8.0),sizeof(unsigned char));
	output->numTicks = numTicks;
	output->messageId = messageId;
	output->message = message;
	output->next = NULL;
	return output;
}

recieverRecord* createRecieverRecord(char type,char status,short int from,long numTicks,int messageId,char* message,char bvc){
	recieverRecord* output = (recieverRecord*)malloc(sizeof(recieverRecord));
	output->type = type;
	output->status = status;
	output->from = from;
	output->bvc = bvc;
	output->bv = (0==bvc)?NULL:(unsigned char*)calloc(ceil(bvc/8.0),sizeof(unsigned char));
	output->numTicks = numTicks;
	output->messageId = messageId;
	output->data = message;
	output->next = NULL;
	return output;
}
