#include<stdio.h>
#include"ecdc.h"

int main(){
	int a = 45;
	result res = encoder((unsigned char*)&a,sizeof(int));
	printf(" R : %d \n",res.numByte);
	return 0;
}
