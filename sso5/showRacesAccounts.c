#include <stdio.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h> //required for ftruncate, which is only in gnu99 standard
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/sem.h>


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

struct buf_t
{
	int accA, accB;
} buf;


int fd;
const char BUFNAME[]="/myregion";
void exiting()
{
	wl(close(fd),__LINE__);
	shm_unlink(BUFNAME);
}

void prooveRaces(int *kontoA, int *kontoB, int seed) {
	#define BEDZIE int *transactions[
	#define TYPOW ][4]
	#define OPERACJI ={

	#define ZROB {(int*)
	#define TRANSAKCJI , 
	#define Z (int*)
	#define KIESZENI NULL
	#define DO , (int*)
	#define KONTA_A kontoA
	#define KONTA_B kontoB
	#define NA , 
	#define KWOTE (int*)
	#define ALE_TO_NIE_KONIEC },
	#define KONIEC }
	#define TO_TYLE };

	int already=seed;

	BEDZIE 2 TYPOW OPERACJI
	ZROB 200000 TRANSAKCJI Z KIESZENI DO KONTA_A NA KWOTE 100 ALE_TO_NIE_KONIEC
	ZROB 200000 TRANSAKCJI Z KONTA_A DO KIESZENI NA KWOTE 100 KONIEC
	TO_TYLE

	assert(already<sizeof transactions/sizeof *transactions);
	int repeats=(int)transactions[already][0];
	int *from=(int*)transactions[already][1];
	int *to=(int*)transactions[already][2];
	int amount=(int)transactions[already][3];
	for (int i = 0; i < repeats; ++i)
	{
		if(from) *from-=amount;
		if(to) *to+=amount;
	}
}

int main(int argc, char const *argv[])
{
	atexit(exiting);
	shm_unlink(BUFNAME);
	fd = wl(shm_open(BUFNAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR /*0774?*/),__LINE__);
	w(fd);
	size_t size = wl(ftruncate(fd, sizeof(buf)),__LINE__);
	fprintf(stderr, "truncated %zu\n", size); //%zu stands for size_t

	struct buf_t *rptr;
	rptr = mmap(NULL, sizeof(buf),
		   PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (rptr == MAP_FAILED) w(-1);

	int processCount=2;
	for (int i = 0; i < processCount; ++i)
	{
		int r=fork();
		if(r==0) { //child
				fprintf(stderr, "Running %d\n", i);
				prooveRaces(&rptr->accA,&rptr->accB, i);
				exit(0);
		} else if (r>0) { //parent
			fprintf(stderr, "\t%s\n", "Parent");
		} else
			wl(r,__LINE__);
	}
	for (int i = 0; i < processCount; ++i)
	{
		int status;
		wait(&status);
	}
	fprintf(stderr, "Na obu kontach w tym momencie winno byc (0,0), a jest (A=%d,B=%d)\n", rptr->accA, rptr->accB);
	//since there is exiting function declared as exit function, we do not need to care about fd descriptor
	return 0;
}