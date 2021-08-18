#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/limits.h>

int
vt_putstamp(char *hostpath, char *run_stamp, int *run_id){
    int err;
    char stampfile[PATH_MAX];
    time_t t = time(0);
    struct tm *lt = localtime(&t);
    FILE *fp;

    sprintf(run_stamp, "%04d%02d%02dT%02d%02d%02d\n", lt->tm_year + 1900, 
            lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec); 
    sprintf(stampfile, "%s/tstamp.log", hostpath);
    fp = fopen(stampfile, "a+");
    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s", run_stamp);
    printf("%ld\n", ftell(fp));
    fclose(fp);
    return 0;
}

int
main(void){
    char timestr[16];
    int rid;
    vt_putstamp(".", timestr, &rid);
    printf("%s\n", timestr);

    return 0;
}
