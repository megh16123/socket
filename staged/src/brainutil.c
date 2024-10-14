#include"ecdc.h"
#include"brainutil.h"

senderRecord* createSenderRecord(unsigned char type,char status,int nr,long numTicks,int messageId,char* message,char bvc,int dsize){
	senderRecord* output = (senderRecord*)malloc(sizeof(senderRecord));
	output->type = type;
	output->status = status;
	output->nr = nr;
	output->bvc = bvc;
	output->bv = (0==bvc)?NULL:(unsigned char*)calloc(ceil(bvc/8.0),sizeof(unsigned char));
	output->numTicks = numTicks;
	output->messageId = messageId;
	output->message = message;
	output->dsize = dsize;
	output->next = NULL;
	return output;
}

recieverRecord* createRecieverRecord(unsigned char type,char status,short int from,long numTicks,int messageId,char* message,char bvc,int dsize){
	recieverRecord* output = (recieverRecord*)malloc(sizeof(recieverRecord));
	output->type = type;
	output->status = status;
	output->from = from;
	output->bvc = bvc;
	output->bv = (0==bvc)?NULL:(unsigned char*)calloc(ceil(bvc/8.0),sizeof(unsigned char));
	output->numTicks = numTicks;
	output->messageId = messageId;
	output->data = message;
	output->dsize=dsize;
	output->next = NULL;
	return output;
}
