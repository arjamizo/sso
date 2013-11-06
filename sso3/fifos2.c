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

int main(int argc, char const *argv[])
{
    char progstr[20][200];
    int cnt=0;
    int opt;
    while ((opt = getopt(argc, argv, "p:sred"))!=-1) {
        switch (opt) {
            case 'p':
            strcpy(progstr[cnt],optarg);
            printf("Read %s\n", optarg);
            cnt++;
            assert(cnt<sizeof progstr/sizeof *progstr);
            break;
            default:
            printf("error\n");
        }
    }
    return 0;
}