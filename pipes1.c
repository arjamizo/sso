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

//Convention: child always reads
// int hookProcess(const char *commandLine, int parentsOutput, int childsOutput) {

// 	int r=fork();
// 	switch(r) {
// 		case 0: 
// 		//wejsciem dziecka jest wyjscie rodzica, 
// 		//zatem nie potrzebujemy wejscia rodzica
// 		printf("Child of %d\n", getppid());
// 		close(stdin);
// 		close(stdout);
// 		close(parentsOutput);
// 		dup(childsOutput); //na
// 		exit(1);
// 		break;
// 		case -1: 
// 		printf("Error\n");
// 		break;
// 		default: 
// 		printf("Process %d\n", r);
// 		close(childsOutput);
// 	}
// }

int main(int argc, char *argv[]) {
	int SIMPLEFORKING=0;
	int opt;
	char progstr[20][200];
	int cnt=0;
	if (argc==1) {
		//DelayRead=SIMPLEFORKING=1;
		printf("WARNING! Overriding run parameters\n");
		#define LEN 2
		argc=LEN;
		char *myargv[LEN]={"pipes1","-pls"
		//,"-p'tee /tmp/txt.txt'","-p'tee /tmp/txt2.txt'","-pwc"
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
	int ppe[2];
	// printf("Running %s\n", progstr[0]);
	//hookProcess definition and //pipes[...][1] is write-only! 
	//extern int hookProcess(const char *commandLine, int stdin, int stdout);

	//int hookProcess(const char *commandLine, int closeInParent, int closeInChild, int toClose1, int toClose2) {
	//wl(pipe(pipes[0]),__LINE__);
	// int cachedInput=dup(stdin),cachedOutput=dup(stdout);

	// wl(pipe(pipes[1]),__LINE__);

	// int r=fork();
	// if(r==0) {
	// 	close(pipes[1][0]);
	// 	dup2(pipes[1][1],STDOUT_FILENO);
	// 	close(pipes[1][1]);
	// 	printf("Nie powinno sie wydarzyc\n");
	// 	execlp("ls","ls","-l",NULL);
	// 	exit(0);
	// } else if (r!=-1) {
	// 	close(pipes[1][1]);
	// 	dup2(pipes[1][0],STDIN_FILENO);
	// 	close(pipes[1][0]);
	// 	char buf;
	// 	while(read(STDIN_FILENO,&buf,1)>0) {
	// 		write(STDOUT_FILENO,&buf,1);
	// 	}
	// 	//close(pipes[1][1]);//nie bedziemy czytac z wejscia do child-a
	// 	// close(STDOUT_FILENO);
	// 	// dup2(pipes[1][0],STDOUT_FILENO);//ale do wejscia child-a bedziemy pisac
	// 	// close(pipes[1][0]);//nie bedziemy pisac do wyjscia z child-a
	// 	// dup2(pipes[0][1],STDIN_FILENO);//ale bedziemy czytac z wyjsica childa
	// }
	// int status;
	// wait(&status);

	//hookProcess(progstr[0],pipes[0][0],pipes[0][1]);//child will not write
	printf("here\n");
	int rdonly=STDIN_FILENO,wronly=STDOUT_FILENO;
	for(int i=0; i<cnt; ++i)
	{
		wl(pipe(pipes[i]),__LINE__);
		//pipes[...][1] is write-only! 
		int prWRO=wronly,prRDO=rdonly;
		rdonly=pipes[i][0];
		wronly=pipes[i][1]; //write only
		int r=fork();
		if(r==0) {
			//child
			// dup2(begin, STDIN_FILENO);
			// close(prBegin);
			// close(prEnd);
			close(rdonly);
			dup2(wronly, STDOUT_FILENO);
			close(wronly);
			execlp("ls","ls","-l",NULL);
			exit(-1);
		} else if (r!=-1) {
			close(wronly);
			dup2(rdonly,STDIN_FILENO);
			close(rdonly);
			// wait(0);
			execlp("wc","wc",NULL);
			exit(-2);
		}
		// hookProcess(progstr[i],pipes[i-1][1],pipes[i][0]);
		printf("Connecting %10s's output to newly started %10s's outputs\n", progstr[i-1], progstr[i]);
	}

	printf("Running last program %s\n", progstr[cnt-1]);
	

	//close(stdout); dup(pipes[0][1]);
	// wl(pipe(pipes[cnt-1]),__LINE__);
	//hookProcess(progstr[cnt-1],pipes[cnt-1-1][1],-1);//Last process will not have changed stdout descriptor
	//this process must be last executed command
	//with set input descriptor to last pipe's out (read-only) pipes[cnt-2][1]
	// close(stdin); dup(pipes[cnt-2][1]);
	for(int i=0; i<cnt-1; ++i) {
		int status;
		//wait(&status);
		printf("process finished %d\n", WEXITSTATUS(status));
	}
	for (int i = 0; i < cnt; ++i)
	{
		// close(pipes[i][0]);
		// close(pipes[i][1]);
	}
}

return 0;
}