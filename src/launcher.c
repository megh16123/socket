#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
int main(int argc, char **argv) {
  if (argc == 2) {
    FILE *config = fopen(argv[1], "r"), *f;
    char *programs[4] = {"mouth", "ear", "brain","ui"};
    char environment[1024][1024];
    int linesRead = 0;
    int pid[3], pi = 0, re;
    int i = 0, j = 0;
    char *bargs[3] = {programs[2], argv[1], NULL}, *eargs[5], *margs[4],
         *uargs[5],**cargs;
    if (NULL == config) {
      printf("File didn't open \n");
    } else {
      while ((environment[i][j] = fgetc(config)) != EOF && linesRead <= 6) {
        if (environment[i][j] == '\n') {
          linesRead += 1;
          environment[i][j] = '\0';
          if (linesRead >= 4 && linesRead <= 7) {
		  if(linesRead==5||linesRead==4 || linesRead==6){
            		f=fopen(environment[i], "wb+");
			fputc('1',f);
			fclose(f);
		  }
		  else {
		  	fclose(fopen(environment[i], "w"));
		 }
          }
          i += 1;
          j = 0;
        } else {
          j += 1;
        }
      }
    }
    eargs[0] = programs[1];
    eargs[1] = environment[1];
    eargs[2] = environment[2];
    eargs[3] = environment[3];
    eargs[4] = NULL;
    margs[0] = programs[0];
    margs[1] = environment[4];
    margs[2] = environment[2];
    margs[3] = NULL;
    uargs[0] = programs[3];
    uargs[1] = environment[5];
    uargs[2] = environment[6];
    uargs[3] = environment[2];
    uargs[4] = NULL;
    ///////////////////////////////////////////////////////////////////////////
	for(i = 0; i<4; i++){
		
		pid[i] = fork();
		if(pid[i] == 0){
			switch(i){
			case 0:
				cargs = margs;
				break;
			case 1:
				cargs = eargs;
				break;
			case 2:
				cargs = bargs;
				break;
			case 3:
				cargs = uargs;
				break;

			}
            		re = execv(programs[i], (char **)cargs);
            		if (re == -1) {
              			printf("Error Running %d\n",i);
            		}
		}
	}
    	sprintf(environment[linesRead],"EXIT_%s",environment[5]);
	while((f= fopen(environment[linesRead],"r")) == NULL);
	kill(pid[0],SIGKILL);
	kill(pid[1],SIGKILL);
	kill(pid[2],SIGKILL);
	kill(pid[3],SIGKILL);
	remove(environment[linesRead]);
     } else {
    printf("Too few arguments given : launcher\n");
  }
  return 0;
}
