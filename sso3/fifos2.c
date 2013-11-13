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
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
void parseParameters(const int argc
    , char * const* argv 
    , int *cnt, char **progstr) {
}
int main(int argc, char *const *argv)
{
    char progstr[20][200];
    int cnt=0;
    // parseParameters(argc, argv
    //     , &cnt
    //     , (char**)progstr);
    int opt;
    int dirty=0;
    while ((opt = getopt(argc, argv, "p:sred"))!=-1) {
        switch (opt) {
            case 'p':
            strcpy(&progstr[cnt][0],optarg);
            cnt++;
            assert(cnt<sizeof progstr/sizeof *progstr);
            break;
            case 'd': 
            dirty=1;
            break;
            default:
            printf("error\n");
        }
    }
    if(!dirty) {
        int new = open("/dev/null", O_WRONLY);
        dup2(new, STDERR_FILENO);
        close(new);
    } else {
        for (int i = 0; i < cnt; ++i)
        {
            fprintf(stderr, "Read %s\n", progstr[i]);
        }
    }

    char fnme[200];
    for (int i = 0; i < cnt; ++i)
    {
        if(i!=cnt-1) {
            snprintf(fnme, sizeof fnme, "/tmp/MKFIFO_%d-%d",i,i+1);
            if(remove(fnme)==-1) {
                perror("File did not exist. Everything OK.");
            }
            wl(mkfifo(fnme, 0666),__LINE__);
            fprintf(stderr,"\tCreated mkfifo: %s\n", fnme);
        }
    }
    // sleep(1);
    int rdonly=STDIN_FILENO,wronly=STDOUT_FILENO;
    for(int i=0; i<cnt; ++i) {
        int r=-1;
        fprintf(stderr,"Loop %d/%d\n", i,cnt-1);
        r=wl(fork(),__LINE__);
        if(r==0) {
            if(i!=0) {
                snprintf(fnme, sizeof fnme, "/tmp/MKFIFO_%d-%d",i-1,i);
                fprintf(stderr,"\tConnected to fifo: %s\n", fnme);
                // Jeśli to nie jest pierwszy proces, to ma on poprzedi, w zwiazku z tym 
                // nalezy obsluzyc wysjscie poprzedniego procesu, czyli STDIN aktualnego
                if(i>0) close(rdonly);
                fprintf(stderr,"\tTHERE1\n");
                rdonly=wl(open(fnme, O_RDONLY),__LINE__);
                fprintf(stderr,"\tTHERE2\n");
                // close(wronly);
                wl(dup2(rdonly,STDIN_FILENO),__LINE__);
                fprintf(stderr,"\tTHERE3\n");
                // close(rdonly);
            }
            if(i!=cnt-1) { //to nie jest ostatni proces, wiec bedzie trzeba go z czyms polaczyc jeszcze
                snprintf(fnme, sizeof fnme, "/tmp/MKFIFO_%d-%d",i,i+1);
                fprintf(stderr,"\tHERE\n");
                if(i>0) close(wronly);
                fprintf(stderr,"\tHERE2 opening %s\n", fnme);
                wronly=wl(open(fnme, O_WRONLY),__LINE__);
                fprintf(stderr,"\tHERE3\n");
                wl(dup2(wronly,STDOUT_FILENO),__LINE__);
                // close(wronly);
                fprintf(stderr,"\tHERE4\n");
            }
            fprintf(stderr,"Loop2 %d/%d\n", i,cnt-1);
            fprintf(stderr, "Creating %s\n", progstr[i]);
            execlp(progstr[i],progstr[i],NULL);
            exit(-1);
            exit(0);
        } else {
            fprintf(stderr, "In parent...\n");
        }
        //r=wl(fork(),__LINE__);
        
        // if(!dirty) 
        // sleep(1);
    }

    for(int i=0; i<cnt; i++) {
        int status;
        wait(&status);
    }
    for(int i=0; i<cnt-1; i++) {
        snprintf(fnme, sizeof fnme, "/tmp/MKFIFO_%d-%d",i,i+1);
        remove(fnme);
    }


    return 0;
}