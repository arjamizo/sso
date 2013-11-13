#include <stdio.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h> //required for ftruncate, which is only in gnu99 standard
#include <sys/types.h>
#include <assert.h>


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
    int i;
} buf;


const char BUFNAME[]="/myregion";
int fd;
void exiting()
{
    wl(close(fd),__LINE__);
    int ret=shm_unlink(BUFNAME);
    // wl(ret,__LINE__);
}

void prooveRaceExists(int *r) {
    int initial=*r;
    // sleep(1);
    int epilogical=*r;
    static int a=0;
    static int b=0;
    a++;
    // if(a++==1000) sleep(2);
    // fprintf(stderr, "initial r=%d epilogical r=%d\n", initial, epilogical);
    // assert(initial==*r);
    if(initial!=epilogical) 
        printf("PROOVED that in %d/%d (%.2f%%) race exists " "initial r=%d epilogical r=%d\n"
            , ++b
            , a
            , (float)100*b/a
            , initial
            , epilogical);
}

int main(int argc, char const *argv[])
{
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

    int r=fork();
    if(r==0) { //child
        fprintf(stderr, "\t\t%s\n", "Child");
        while(1)
            prooveRaceExists(&rptr->i);
        exit(0);
    } else if (r>0) { //parent
        fprintf(stderr, "\t%s\n", "Parent");
        // sleep(1);
        while(1) {
            rptr->i++;
            // sleep(1);
        }
        int status;
        wait(&status);
    } else 
        wl(r,__LINE__);

    return 0;
}