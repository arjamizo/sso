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

/***
* Przypadki testowe: 
    - nie ma kogos, kto by czytal
    - nie ma kogos, kto by pisal
    - ktos pisze za duzo, albo 
*/
int main(int argc, char const *argv[])
{
    int ppe[2];
    wl(pipe(ppe),__LINE__);
    int rdonly=ppe[0];
    int wronly=ppe[1];
    int r=fork();
    switch(r) {
        case -1: 
        perror("error!");
        break; 
        case 0: 
        {
            //child
            close(wronly); 
            int i;
            for (i = 0; i < 2; ++i)
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
                while((readed=read(rdonly, (void*)buf, 1+0*sizeof buf))>0) {
                    totread+=readed;
                    fprintf(stderr, "read %d in total\n", totread);
                    if(firstread--<0)getchar();
                }
            }
            fprintf(stderr, "FINISHED\n");
            sleep(10);
            close(rdonly);
        }
        break;
        default:
        { 
            close(rdonly);
            int i;
            for (i=0; i<(1<<6)+5; ++i) {
                char buf[1<<10];
                wl(write(wronly, buf, sizeof buf),__LINE__);
                fprintf(stderr, "wrote %d in total\n", (int)sizeof buf * (i+1));
            }
            sleep(10);
            close(wronly);
            //parent, child ID in int r
        }
    }
    return 0;
}