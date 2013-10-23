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


int KillRead=0, EagerClose=0;

/**
* Funkcja uruchamiana w procesie dziecka, ktore czyta.
* uzycie takiej konwencji argumentow moge uargumentowac tym,
* ze jest ono wysoce eleganckie, gdyz funkcja implementujaca
* dziecko ogranicza swoim dzialaniem
*/

int child(int input) {
	char buf[200];
	//char buf;
	int totread=0;
	printf("inChild\n");
	if (KillRead) exit(1);
	int rlimit=2;
	if (DelayRead) for(int i=0; i<rlimit; ++i) {printf("delaying read %d/%d\n", i, rlimit); sleep(1);}
	if (EagerClose) {close(input); printf("Warning! Not reading, even more, closed at the beginnig\n"); while(1);}
	sleep(1);
	while(1) {
		int readed=0;
		while((readed=read(input, (void*)buf, sizeof buf-1))>0) {
			totread+=readed;
			readed=1;
			buf[readed]='\0';
	#ifdef MillionTest
			printf("readed %d, so far %d\n", readed, totread);
	#else
			printf("%s", buf);
	#endif
		}
		wl(readed,__LINE__);
		if(!readed) printf("nothing readed in this turn\n");
		sleep(2);
	}
	close(input);
	return 0;
}

int parent(int output) {
	//  const char *t="czesc Uzytkowniku\n";
	char buf='T';
	printf("ELO\n");
	//write(output, t, strlen(t));
	#ifdef MillionTest
	printf("Warning! In million test mode!\n");
	sleep(2);

	for(int i=0; i<(int)10e5 /*or 100000*/; ++i) {
		// if(i%20000==0 || i>65534) 
		{
			printf("wrote %d\n", i);
			sleep(1);
		}
		wl(write(output, &buf, 1),__LINE__);
	}
	#else
	printf("reading from input\n");
	while((buf=getchar())!=EOF) {
		w(write(output, (const void*)&buf, 1));
		printf("Writing %c\n",buf);
	}
	#endif
	close(output);
	return 0;
}


int main(int argc, char *argv[]) {
	int SIMPLEFORKING=0;
	int opt;
	char progstr[20][200];
	int cnt=0;
	if (argc==1) {
		//DelayRead=SIMPLEFORKING=1;
		printf("WARNING! Overriding run parameters\n");
		#define LEN 4
		argc=LEN;
		char *myargv[LEN]={"pipes1","-pls"
		//,"-p'tee /tmp/txt.txt'"
		//,"-p'tee /tmp/txt2.txt'"
		,"-pwc"
		,"-pwc"
		};
		argv=(char**)myargv;
	}
	while ((opt = getopt(argc, argv, "p:sre"))!=-1) {
		switch (opt) {
			case 's':
			SIMPLEFORKING=1;
			break;
			case 'r':
			printf("WARNING! Delaying Reading\n");
			sleep(1);
			DelayRead=1;
			break;
			case 'e':
			printf("EagerClose mode on\n");
			EagerClose=1;
			break;
			case 'k':
			printf("warning! Killing reading process.");
			KillRead=1;
			break;
			case 'p':
			strcpy(progstr[cnt],optarg);
			printf("added new program to run-list: %s\n", progstr[cnt]);
			cnt++;
			#ifdef DEBUG
			printf("new d=%s\n",optarg);
			#endif
			break;
			default:
			printf("error\n");
		}
	}


if(SIMPLEFORKING) {
	int pipefd[2];
	wl(pipe(pipefd),__LINE__); //pipefd[1] is read-only, pipefd[0] is write-only
	int r=fork();
	switch(r) {
		case 0:
		printf("STARTED CHILD\n");
		#ifndef DontCloseOutputOnChild
		close(pipefd[1]);
		#endif
		child(pipefd[0]);
		exit(0);
		break;
		case -1:
		printf("deadbeef\n");
		exit(0);
		break;
		default:
		printf("created child with PID=%d\n", r);
		#ifndef DontCloseInputInParent
		close(pipefd[0]);
		#endif
		printf("elo\n");
		parent(pipefd[1]);
	}
	int status;
	w(wait(&status));
	printf("FINISHED \n");
} else {
	printf("passing parameters\n");

	if(cnt>10) {exit(10); }//too many arguments
	int pipes[10][2]; 
	printf("here, %d\n", argc-1);
	int rdonly=STDIN_FILENO,wronly=STDOUT_FILENO;
	// int prWRO=wronly,prRDO=rdonly;
	for(int i=0; i<cnt; ++i)
	{
		fprintf(stderr, "In loop: running %s\n", progstr[i]);
		wl(pipe(pipes[i]),__LINE__);
		//pipes[...][1] is write-only! 
		rdonly=pipes[i][0];
		wronly=pipes[i][1]; //write only
		int r=fork();
		if(r==0) {
			fprintf(stderr, "In child: running %s\n", progstr[i]);
			//child

			//ZAKLADAMY, ze pierwsze uruchomienie ma w kontekscie STDIN
			close(rdonly);
			dup2(wronly, STDOUT_FILENO);
			close(wronly);
			execlp(progstr[i],progstr[i],NULL);
			exit(-1);
		} else if (r!=-1) {
			fprintf(stderr, "In parent: running %s, i=%d/%d\n", progstr[i], i, cnt-1);
			close(wronly);
			dup2(rdonly,STDIN_FILENO); //to wywolanie dup2 operuje na pipie starym
			close(rdonly);
			if(i==cnt-1)
				execlp("cat","cat",NULL);
			sleep(1);
		}
		wl(r,__LINE__);
		// hookProcess(progstr[i],pipes[i-1][1],pipes[i][0]);
		printf("Connecting %10s's output to newly started %10s's input\n", i==0?"this process":progstr[i-1], progstr[i]);
	}

	printf("Running last program %s\n", progstr[cnt-1]);
	
	//with set input descriptor to last pipe's out (read-only) pipes[cnt-2][1]
	for(int i=0; i<cnt-1; ++i) {
		int status;
		wait(&status);
		printf("process finished %d\n", WEXITSTATUS(status));
	}
}

return 0;
}