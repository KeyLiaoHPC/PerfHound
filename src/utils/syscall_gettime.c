#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

int
main(int argc, char **argv) {
    
    uint64_t val[20];

    struct timespec foo;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME_HR, &foo );
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    printf("%llu, %llu\n", foo.tv_sec, foo.tv_nsec);
    val[0] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    printf("%llu, %llu\n", foo.tv_sec, foo.tv_nsec);
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[1] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[2] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[3] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[4] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[5] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[6] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[7] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[8] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[9] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[10] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[11] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[12] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[13] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[14] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[15] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[16] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[17] = foo.tv_sec * 1e9 + foo.tv_nsec;
//    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[18] = foo.tv_sec * 1e9 + foo.tv_nsec;
    syscall( __NR_clock_gettime, CLOCK_REALTIME, &foo );
//    clock_gettime(CLOCK_MONOTONIC, &foo);
    val[19] = foo.tv_sec * 1e9 + foo.tv_nsec;
    for (int i = 1; i < 20; i += 2)
        printf("%llu\n", val[i] - val[i-2]);
    for (int i = 2; i < 20; i += 2)
        printf("%llu\n", val[i] - val[i-2]);

    return 0;
}
