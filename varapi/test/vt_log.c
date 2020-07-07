#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>

/* Logging */
void
vt_log(FILE *fp, char *fmt, ...) {
    /* Generate natural timestamp. */
    time_t t = time(0);
    struct tm *lt = localtime(&t);
    char msg[1024];

    // <YYYY><MM><DD>T<hh><mm><ss>,<msg>
    sprintf(msg, "%04d%02d%02dT%02d%02d%02d,", lt->tm_year + 1900, lt->tm_mon + 1,
            lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

    va_list args;
    va_start(args, fmt);
    vsprintf(&msg[16], fmt, args);
    va_end(args);

    fprintf(fp, msg);
}

int
main(void) {
    FILE *fp;
    int x = 20;

    fp = fopen("vtlog.log", "w");
    vt_log(fp, "This is a test. %d", x);
    printf("This is a test. %d\n", x);
    fclose(fp);

    return 0;
}
