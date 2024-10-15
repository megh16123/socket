#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char **argv) {
  if (argc == 2) {
    FILE *config = fopen(argv[1], "r"), *f;
    char *programs[4] = {"mouth", "ear", "ui", "brain"};
    char environment[1024][1024];
    int linesRead = 0;
    int pid[3], pi = 0, re;
    int i = 0, j = 0;
    char *bargs[3] = {programs[3], argv[1], NULL}, *eargs[5], *margs[4],
         *uargs[4],**cargs;
    if (NULL == config) {
      printf("File didn't open \n");
    } else {
      while ((environment[i][j] = fgetc(config)) != EOF && linesRead <= 6) {
        if (environment[i][j] == '\n') {
          linesRead += 1;
          environment[i][j] = '\0';
          if (linesRead >= 4 && linesRead <= 7) {
            fclose(fopen(environment[i], "w"));
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
    uargs[0] = programs[2];
    uargs[1] = environment[5];
    uargs[2] = environment[6];
    uargs[3] = NULL;
    ///////////////////////////////////////////////////////////////////////////
	for(i = 0; i<3; ){
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
				cargs = uargs;
				break;
			case 3:
				cargs = bargs;
				break;

			}
            		printf("LAUNCH :%s %s %d\n",cargs[0], programs[i], i);
            		re = execv(programs[i], (char **)cargs);
            		if (re == -1) {
              			printf("Error Running %d\n",i);
            		}
		}else{
		printf("I :%d\n",i);
			i++;
		}
	}
//while(1);
             		printf("LAUNCH : %s %d\n", programs[i], i);
             		re = execv(programs[i], (char **)bargs);
             		if (re == -1) {
               			printf("Error Running %d\n",i);

			}
     } else {
    printf("Too few arguments given : launcher\n");
  }
  return 0;
}
