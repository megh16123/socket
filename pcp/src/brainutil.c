#include"ecdc.h"
#include"brainutil.h"
extern senderRecord* senderTable;
senderRecord* createSenderRecord(char type,char status,nRecord* nr,long numTicks,result messageId,char* message){
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
//  iterate 
void doJob(){
	senderRecord* temp = senderTable,*prev;
	do{
	prev = temp;
	temp = temp->next;
	if(temp->numTicks != 0)
	{
		temp->numTicks--;
	}else{
		continue;
	}
// 	process the data

// if delete 
// 	prev->next = temp->next;
	}while(temp != senderTable);
}
