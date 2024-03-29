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
    int i,a,b;
} buf;


const char BUFNAME[]="/myregion";
int fd;
void exiting()
{
    wl(close(fd),__LINE__);
    // int ret=
    shm_unlink(BUFNAME);
    // wl(ret,__LINE__);
}

#define KEY ((key_t) 123456L) //could be generated by semkey
#define PERM 0600
int semkey;
int initSem() {
    semkey=wl(semget(semkey, 1, 0666 | IPC_CREAT),__LINE__);
    short initarray[1]={1};
    wl(semctl(semkey, 1, SETALL, initarray),__LINE__);
    return semkey;
}

void takeSem() {
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    wl(semop(semkey, &sops, 1),__LINE__);
}

void releaseSem() {
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    wl(semop(semkey, &sops, 1),__LINE__);
}

void freeSem() {
    semctl(semkey, 1, IPC_RMID );
}

int main(int argc, char const *argv[])
{
    int useSemaphores=argc>1;
    if(argc==1) {
        fprintf(stderr, "Run with --semaphores to use kernel semaphores.\n");
        fprintf(stderr, "Now running WITHOUT semaphores\n");
        useSemaphores=0;
    } else {
        fprintf(stderr, "Now running WITH semaphores\n");
        useSemaphores=1;
    }
    atexit(exiting);
    shm_unlink(BUFNAME);
    fd = wl(shm_open(BUFNAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR /*0774?*/),__LINE__);
    // fd = shm_open("/myregion", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    w(fd);
    size_t size = wl(ftruncate(fd, sizeof(buf)),__LINE__);
    fprintf(stderr, "truncated %zu\n", size); //%zu stands for size_t

    struct buf_t *rptr;
    rptr = mmap(NULL, sizeof(buf),
           PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (rptr == MAP_FAILED) w(-1);
    if(useSemaphores) initSem();
    int r=fork();
    printf("i=%d a=%d b=%d\n", rptr->i, rptr->a, rptr->b);
    size_t try=20000;
    if(r==0) { //child
        fprintf(stderr, "\t\t%s\n", "Child");
        for (int i = 0; i < try; ++i) {
            if(useSemaphores) takeSem();
            rptr->b++;
            rptr->i++;
            usleep(5);
            if(useSemaphores) releaseSem();
        }
        exit(0);
    } else if (r>0) { //parent
        fprintf(stderr, "\t%s\n", "Parent");
        for (int i = 0; i < try; ++i) {
            if(useSemaphores) takeSem();
            rptr->a++;
            rptr->i++;
            usleep(5);
            if(useSemaphores) releaseSem();
        }
        int status;
        wait(&status);
    } else 
        wl(r,__LINE__);
    if(useSemaphores) freeSem();
    printf("\ti=%d a=%d b=%d\n", rptr->i, rptr->a, rptr->b);
    assert(rptr->i==try*2);
    return 0;
}