#include <stdio.h>
#include "ecdc.h"

#define MAX_FNAME 256
static int BUFFLEN;
FILE* f;
#define printline printf("\n---------------------------------------------------\n");
#define Fetch_Print 		sendTobrain(argv[1], buf,argv[2]);\
				getFromBrain(argv[2]);\
				printline
int dex = 0,dsize=0;
void sendTobrain(char *fname,unsigned char *buf,char *fname2) {      
	f = fopen(fname,"rb+");
	unsigned char ch;
	fseek(f,0,SEEK_SET);
	while((ch = fgetc(f))!='1'){
		fclose(f);
		f = fopen(fname,"rb+");
		fseek(f,0,SEEK_SET);
	}
	//TODO: if domain of ch expands insert if ch == 1  here

	ch = '0';
	fseek(f,1,SEEK_SET);
	fwrite(buf,sizeof(unsigned char),BUFFLEN,f);
	fseek(f,0,SEEK_SET);
	fputc(ch,f);
	fclose(f);
	f = fopen(fname2,"rb+");
   	fseek(f,0,SEEK_SET);
	dsize=-1;
   	fwrite(&dsize,sizeof(int),1,f);
	fclose(f);
}


void getFromBrain(char *fname){
	
	int ch;
	dex=0;
	char c;
	FILE *f = fopen(fname,"rb+");
   	fseek(f,0,SEEK_SET);
   	fread(&ch,sizeof(int),1,f);
   	while(ch == -1){
		fclose(f);
		f = fopen(fname,"rb+");
   		fseek(f,0,SEEK_SET);
   		fread(&ch,sizeof(int),1,f);
	}
	if(ch>1){printline;printf("%d: ",dex);}
	for(int i=0;i<ch-2;i++)
	{
		c = fgetc(f);
		if(c=='\n')printf("\n%d: ",++dex);
		else printf("%c",c);
	}
	fclose(f);
	f = fopen(fname,"wb");
  	fclose(f);
}


int main(int argc, char **argv)
{
	if (argc == 4) {
		BUFFLEN = atoi(argv[3]);
		unsigned char buf[BUFFLEN];
		char fname[MAX_FNAME];
		char choice,ch,flg = 0;
		int i,j;
		char *message;
		FILE *tobrain, *frombrain,*f;
		 
		while (flg == 0) { 
			printf("\n1. Send Message\n2. List Messages\n3. List Systems\n4. Exit\n\nEnter Choice: "); choice = getchar();
			getchar();
			clm(buf);
			switch (choice) {
			case '1':
				*buf = '3';
				Fetch_Print
				printf("TO (index) : ");
				scanf("%d",&i);
				getchar();
				if(i < 0 || dex < i){
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
				*((int*)(buf+1)) = i;
				fseek(f,0,SEEK_END);
				*((int*)(buf+sizeof(int)+1)) = ftell(f); 
				strcpy((buf+(2*sizeof(int))+1),fname);
				fclose(f);
				sendTobrain(argv[1], buf,argv[2]);
				getFromBrain(argv[2]);
				printline;
				break;
			case '2':
				*buf = choice;
				Fetch_Print
				break;
			case '3':
				*buf = choice;
				Fetch_Print
				break;

			case '4':
				flg = 1;
				sprintf(fname,"EXIT_%s",argv[1]);
				fclose(frombrain);
				fclose(fopen(fname,"w"));
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
