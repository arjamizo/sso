#include <unistd.h>

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int DelayRead=0;

/**
* function is wrapper for errors
*/
int w(int execcode) {
	if (execcode<0) {
		perror("error somewhere");
	}
	return execcode;
}
/**
* wrapper for errors with line handling
*/
#include <assert.h>
int wl(int execcode, int line) {
	static char msg[200];
	if (execcode<0)
	{
		assert(1);
		sprintf(msg, "error somewhere in line %d", line);
		perror(msg);
	}
	return execcode;
}
#define fnme "/tmp/KOLEJKA"
/***
* Przypadki testowe: 
	- nie ma kogos, kto by czytal
	- nie ma kogos, kto by pisal
	- ktos pisze za duzo - w tym wypadku po ilu odczytanych bajtach mozna pisac dalen 
*/
int main(int argc, char const *argv[])
{
    int f=mkfifo(fnme, 0666);
    wl(f,__LINE__);
	int r=fork();
	switch(r) {
		case -1: 
		perror("error!");
		break;
		case 0:
			//child
			for (int i = 0; i < 2; ++i)
			{
				sleep(1);
				printf("waiting...");
				fflush(stdout);
			}
			printf("\n");
			{
				int readed,totread=0;
				char buf[1<<10];
				int firstread=4090;
				int rdonly=open(fnme, O_RDONLY);//RDONLY byc nie moze, czeka az ktos bedzie chcial czytac
				wl(rdonly,__LINE__);
				while((readed=read(rdonly, (void*)buf, 1+0*sizeof buf))>0) {
					totread+=readed;
					if(firstread--<0){
						fprintf(stderr, "read %d in total\n", totread);
						getchar();
					}
				}
				close(rdonly);
			}
			fprintf(stderr, "FINISHED\n");
			sleep(10);
		break;
		default: 
		{
			int wronly=open(fnme, O_WRONLY);//RDONLY byc nie moze, czeka az ktos bedzie chcial czytac
			wl(wronly,__LINE__);
			for (int i=0; i<(1<<6)+5; ++i) {
				char buf[1<<10];
				wl(write(wronly, buf, sizeof buf),__LINE__);
				fprintf(stderr, "wrote %d in total\n", (int)sizeof buf * (i+1));
			}
			close(wronly);
			sleep(10);
			//parent, child ID in int r
		}
	}
	return 0;
}