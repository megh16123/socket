#include <stdio.h>
#include "ecdc.h"

#define MAX_FNAME 256
#define BUFFLEN 1024
#define printline printf("\n---------------------------------------------------\n");
#define Fetch_Print 		sendTobrain(argv[1], buf);\
				getFromBrain(frombrain, buf);\
				printline\
				printf(" Total: %d",*((int*)buf));\
				printline\
				i = 0,j = 4;\
				while (i <= *((int*)buf)) {\
					ch = *(buf+j);\
					if(ch != '\n')\
						printf("%c", ch);\
					else{\
						if(i != (*((int*)buf))) printf("\n %d: ", i);\
						i++;\
					}\
					j++;\
				}\
				printline
int dsize = 0,new=0,old=0;
void sendTobrain(char *fname, char *buf) {      
	FILE *f = fopen(fname,"ab");
	fwrite(buf, 1, BUFFLEN, f);
	fclose(f);
}

void getFromBrain(FILE * f, char *buf) {
	char flg = 0;
	while (flg == 0){
		fseek(f, 0, SEEK_END);
		new = ftell(f);
		dsize = new - old;
  		if (dsize > 4) {
			flg = 1;
	    		fseek(f, old, SEEK_SET);
			fread(buf, 1, BUFFLEN, f);
			old = new;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc == 3) {
		char buf[BUFFLEN],fname[MAX_FNAME];
		char choice,ch;
		int i,j,index;
		char *message;
		FILE *tobrain, *frombrain,*f;
		while ((frombrain = fopen(argv[2], "rb")) == NULL)continue;
		new = ftell(frombrain);
		old = new; while (1) { printf("\n1. Send Message\n2. List Messages\n3. List Systems\n4. Exit\n\nEnter Choice: "); choice = getchar();
			getchar();
			clm(buf);
			switch (choice) {
			case '1':
				*buf = '3';
				Fetch_Print
				printf("TO (index) : ");
				scanf("%d",&index);
				getchar();
				if(index < 0 || index > i){
					printf("No such system exists\n");
					break;
				}
				clm(fname);
				printf("Enter the filename : ");
				gets(fname);
				f = fopen((const char*)fname,"rb");
				if(NULL == f){
					printf("File doesn't exist\n");	
					break;
				}
				*buf = '1';
				*((int*)(buf+1)) = index;
				fseek(f,0,SEEK_END);
				*((int*)(buf+sizeof(int)+1)) = ftell(f); 
				strcpy((buf+(2*sizeof(int))+1),fname);
				fclose(f);
				sendTobrain(argv[1], buf);
				getFromBrain(frombrain, buf);
				if(127 ==*buf){
					printf("Message Sent\n");
				
				}else{
					printf("Message Could not be Sent \n");	
				}
				printline
				break;
			case '2':
				*buf = choice;
				Fetch_Print
				break;
			case '3':
				*buf = choice;
				Fetch_Print
				break;

			case '4':// exit by closing all process
			       	break;

			default:
				printf("Invalid Choice\n");
			}
		}
	} else {
		printf("UI: Too few arguments\n");
	}
	return 0;
}
