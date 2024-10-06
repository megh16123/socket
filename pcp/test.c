#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
int main(){
	FILE *f = fopen("test","wb+");
	fseek(f,-ftell(f),SEEK_SET);
	fputc('Z',f);	

	fclose(f);
	return 0;
}
