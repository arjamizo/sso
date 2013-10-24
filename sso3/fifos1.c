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
int wl(int execcode, int line) {
	static char msg[200];
	if (execcode<0)
	{
		sprintf(msg, "error somewhere in line %d", line);
		perror(msg);
	}
	return execcode;
}

/***
* Przypadki testowe: 
	- nie ma kogos, kto by czytal
	- nie ma kogos, kto by pisal
	- ktos pisze za duzo, albo 
*/
int main(int argc, char *argv[]) {
	int ppe[2];
	wl(pipe(ppe),__LINE__);
	int rdonly=ppe[0];
	int wronly=ppe[1];
	int r=fork();
	int waitingForReadAfterReadingMagicalNumber=2;
	int waitinfForReading=1;
	if(r==0) {
		printf("Proces czytajacy: Proces dziecka: czyta\n");
		int readed=0;
		char buf[1<<16];
		close(wronly);
		int waitinfForReading=1;
		printf("Proces czytajacy: Czekanie %d sekund%s, aby bylo co czytac\n", waitinfForReading, (waitinfForReading==1)?"y":"");
		for (int i = 0; i < waitinfForReading; ++i)
		{
			sleep(1);
		}
		printf("Proces czytajacy: Przeczytano magiczna liczbe %d z bufora o dl %d.\n", (int)read(rdonly,(void*)buf,sizeof buf), sizeof buf);
		sleep(waitingForReadAfterReadingMagicalNumber);
		int toRead=1;
		int slowMotionAfterRead=4096;
		if(argc==2) {
			toRead=atoi(argv[1]);
			printf("Proces czytajacy: parametr dostrajania czytania=%d\n", toRead);
		}
		int totread=0;
		while((readed=read(rdonly, (void*)buf, toRead))>0) {
			totread+=readed;
			// buf[readed]='\0';
			// if(totread>65534 && totread<65534+4096) {
				// if(totread%slowMotionAfterRead<2 && totread%slowMotionAfterRead>slowMotionAfterRead-2) 
				printf("Proces czytajacy: read %d, so far %d, a-b=%d\n", readed, totread, totread-readed);
				// if(totread%slowMotionAfterRead<20 && totread%slowMotionAfterRead>slowMotionAfterRead-20) sleep(1);
			// }
		}
		wl(readed,__LINE__);
	} else if (r!=-1) {
		close(rdonly);
		char buf[2048];
		// for(int i=0; i<(int)10e5 /*or 100000*/; i+=sizeof buf) {
		int n=65535;
		printf("Trwa pisanie %d bajtów\n", n);
		for (int i = 0; i < n; ++i) {
			wl(write(wronly, buf, 1),__LINE__);
			if(i+2==n) printf("Prawdopodobnie teraz nastepuje czekanie na odczyt, bo pipe zostal zapelniony w 100%%=%d bajtow\n", i+1);
		}
		printf("Oczekujemy %ds aby odczytano odpowiednia ilosc bajtow\n", waitingForReadAfterReadingMagicalNumber);
		for (int i = 0; i < waitingForReadAfterReadingMagicalNumber; ++i) {
			sleep(1);
			printf(".");
			fflush(stdout);
		}
		printf("\n");
		for(int i=0; i<80*(int)1e3 /*or 100000*/; i+=sizeof buf) {
			int bl=1<<10<<2; //==4096
			// printf("writing i=%d\n", i);
			wl(write(wronly, buf, sizeof buf),__LINE__);
			// if((i>65534 && i<65534+6) || (i>65534+bl && i<65534+12+bl)) 
			{
			// if(i>65534 || i<65534+10) {
				printf("wrote %d; -(1<<16)=%d==%d\n", i, n, i-n);
				// sleep(1);
			}
		}

	} else exit(-1);
}
