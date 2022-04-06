/**
 *  A altered STREAM-MPI for Pfh-Probe self tests.
 *  Scaling up from 1 KiB to 512 MiB per rank.
 */
# define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"
#include "varapi.h"


#ifdef NTIMES
#if NTIMES<=1
#define NTIMES    10
#endif
#endif
#ifndef NTIMES
#define NTIMES    1000
#endif

#ifndef SCALAR
#define SCALAR 0.42
#endif


#ifndef OFFSET
#define OFFSET    0
#endif


# define HLINE "-------------------------------------------------------------\n"

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif

#define STREAM_TYPE double
#define _read_ns(_ns, _ts)   clock_gettime(CLOCK_MONOTONIC, &(_ts));    \
                        (_ns) = _ts.tv_sec * 1e9 + _ts.tv_nsec;
double *restrict a, *restrict b, *restrict c;

size_t array_elements, array_bytes, array_alignment;
static uint64_t avgtime[4] = {0}, maxtime[4] = {0}, mintime[4] = {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX};

static char    *label[4] = {"Copy:      ", "Scale:     ", "Add:       ", "Triad:     "};

static double    bytes[4] = {
    2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE
    };

extern void checkSTREAMresults(STREAM_TYPE *AvgErrByRank, int numranks);
extern void computeSTREAMerrors(STREAM_TYPE *aAvgErr, STREAM_TYPE *bAvgErr, STREAM_TYPE *cAvgErr);

void warmup(double *restrict a, double *restrict b, double *restrict c, uint64_t n) {
	struct timespec ts;
    uint64_t t0, t1, j;
    _read_ns(t0, ts);
    t1 = t0 + 1e9;
    while (t0 < t1) {
        for (j=0; j<n; j++)
            c[j] = a[j];
        for (j=0; j<n; j++)
            b[j] = SCALAR * c[j];
        for (j=0; j<n; j++)
            c[j] = a[j] + b[j];
        for (j=0; j<n; j++)
            a[j] = b[j] + SCALAR * c[j];
        _read_ns(t0, ts);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

int
main() {
    int            quantum, checktick();
    int            BytesPerWord;
    int            i, k, group;
    ssize_t        j;
    STREAM_TYPE        scalar;
    uint64_t t, times[4][NTIMES];
    uint64_t *TimesByRank;
    int         rc, numranks, myrank;
    STREAM_TYPE AvgError[3] = {0.0,0.0,0.0};
    STREAM_TYPE *AvgErrByRank;
    uint64_t cur_bytes, t0, t1, tmin;
    struct timespec ts;
    FILE *fp;
    char myfile[PATH_MAX];
    char stream_tag[8][32] = {"copy_st", "copy_en", "scale_st", "scale_en", "add_st", "add_en", "triad_st", "triad_en"};

    /* --- SETUP --- call MPI_Init() before anything else! --- */

    rc = MPI_Init(NULL, NULL);
    t0 = MPI_Wtime();
    if (rc != MPI_SUCCESS) {
       printf("ERROR: MPI Initialization failed with return code %d\n",rc);
       exit(1);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    /* Pfh-Probe init. */
    if (vt_init("./varapi_stream_self", "st_mpi_1node")) {
        exit(1);
    }
    vt_commit();

    /* Scaling from 1KiB to 10240KiB*/
    cur_bytes = 64;
    scalar = SCALAR;
    group = 0;
    if (myrank == 0) {
        // There are 3 average error values for each rank (using STREAM_TYPE).
        AvgErrByRank = (double *) malloc(3 * sizeof(double) * numranks);
        if (AvgErrByRank == NULL) {
            printf("Ooops -- allocation of arrays to collect errors on MPI rank 0 failed\n");
            MPI_Abort(MPI_COMM_WORLD, 2);
        }
        memset(AvgErrByRank,0,3*sizeof(double)*numranks);
    
        // There are 4*NTIMES timing values for each rank (always doubles)
        TimesByRank = (uint64_t *) malloc(4 * NTIMES * sizeof(uint64_t) * numranks);
        if (TimesByRank == NULL) {
            printf("Ooops -- allocation of arrays to collect timing data on MPI rank 0 failed\n");
            MPI_Abort(MPI_COMM_WORLD, 3);
        }
        memset(TimesByRank,0,4*NTIMES*sizeof(uint64_t)*numranks);
    }

    while (cur_bytes < 10485760) {
        /* Set tags for Pfh-Probe */
        char tag_str[256];
        group ++;
        sprintf(tag_str, "%llubytes", cur_bytes);
        vt_set_grp(group, tag_str);
        for (i = 0; i < 8; i ++) {
            vt_set_tag(group, i + 1, stream_tag[i]);
        }

        /* Initailization */
        array_elements = cur_bytes / sizeof(double);
        array_alignment = 64;
        array_bytes = array_elements * sizeof(double);
        k = posix_memalign((void **)&a, array_alignment, array_bytes);
        if (k != 0) {
            printf("Rank %d: Allocation of array a failed, return code is %d\n",myrank,k);
            MPI_Abort(MPI_COMM_WORLD, 2);
            exit(1);
        }
        k = posix_memalign((void **)&b, array_alignment, array_bytes);
        if (k != 0) {
            printf("Rank %d: Allocation of array b failed, return code is %d\n",myrank,k);
            MPI_Abort(MPI_COMM_WORLD, 2);
            exit(1);
        }
        k = posix_memalign((void **)&c, array_alignment, array_bytes);
        if (k != 0) {
            printf("Rank %d: Allocation of array c failed, return code is %d\n",myrank,k);
            MPI_Abort(MPI_COMM_WORLD, 2);
            exit(1);
        }
        if (myrank == 0) {
            printf("================START %llu Bytes, %llu elements ===================\n", cur_bytes, array_elements);
            printf("STREAM version $Revision: 1.8 $\n");
            printf(HLINE);
        }
        for (j=0; j<array_elements; j++) {
            a[j] = 1.0;
            b[j] = 2.0;
            c[j] = 0.0;
        }
        
        /* Blank STREAM */
        for (j = 0; j < 4; j ++) {
            avgtime[j] = 0;
            mintime[j] = UINT64_MAX;
            maxtime[j] = 0;
        }
        warmup(a, b, c, array_elements);
        for (j=0; j<array_elements; j++) {
            a[j] = 1.0;
            b[j] = 2.0;
            c[j] = 0.0;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        for (k = 0; k < NTIMES; k ++) {
            // kernel 1: Copy
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            for (j=0; j<array_elements; j++)
                c[j] = a[j];
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[0][k] = t1 - t0;

            // kernel 2: Scale
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            for (j=0; j<array_elements; j++)
                b[j] = scalar*c[j];
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[1][k] = t1-t0;
    
            // kernel 3: Add
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            for (j=0; j<array_elements; j++)
                c[j] = a[j]+b[j];
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[2][k] = t1-t0;
    
            // kernel 4: Triad
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            for (j=0; j<array_elements; j++)
                a[j] = b[j]+scalar*c[j];
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[3][k] = t1-t0;
        }
        /*    --- SUMMARY --- */

        // Gather all timing data to MPI rank 0
        MPI_Gather(times, 4*NTIMES, MPI_UINT64_T, TimesByRank, 4*NTIMES, MPI_UINT64_T, 0, MPI_COMM_WORLD);

        if (myrank == 0) {
            for (k=0; k<NTIMES; k++) {
                for (j=0; j<4; j++) {
                    tmin = UINT64_MAX;
                    for (i=0; i<numranks; i++) {
                        // printf("DEBUG: Timing: iter %d, kernel %lu, rank %d, tmin %f, TbyRank %f\n",k,j,i,tmin,TimesByRank[4*NTIMES*i+j*NTIMES+k]);
                        tmin = MIN(tmin, TimesByRank[4*NTIMES*i+j*NTIMES+k]);
                    }
                    // printf("DEBUG: Final Timing: iter %d, kernel %lu, final tmin %f\n",k,j,tmin);
                    times[j][k] = tmin;
                }
            }

            for (k=0; k<NTIMES; k++) {
                for (j=0; j<4; j++) {
                    avgtime[j] = avgtime[j] + times[j][k];
                    mintime[j] = MIN(mintime[j], times[j][k]);
                    maxtime[j] = MAX(maxtime[j], times[j][k]);
                }
            }

            printf("Function    Best Rate B/ns  Avg time     Min time     Max time\n");
            for (j=0; j<4; j++) {
                avgtime[j] = avgtime[j]/NTIMES;

                printf("%s%11.6f  %llu  %llu  %llu\n", label[j],
                   (double)bytes[j]/(double)mintime[j],
                   avgtime[j],
                   mintime[j],
                   maxtime[j]);
            }
            printf(HLINE);
        }
        sprintf(myfile, "./data/%d_%llubytes_0.csv", myrank, cur_bytes);
        fp = fopen(myfile, "w");
        for (j = 0; j < NTIMES; j ++) {
            fprintf(fp, "%llu,%llu,%llu,%llu\n", times[0][j], times[1][j], times[2][j], times[3][j]);
        }
		fflush(fp);
		fclose(fp);
        /* --- Every Rank Checks its Results --- */
        computeSTREAMerrors(&AvgError[0], &AvgError[1], &AvgError[2]);
        /* --- Collect the Average Errors for Each Array on Rank 0 --- */
        MPI_Gather(AvgError, 3, MPI_DOUBLE, AvgErrByRank, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        /* -- Combined averaged errors and report on Rank 0 only --- */
        if (myrank == 0) {
            checkSTREAMresults(AvgErrByRank,numranks);
            printf(HLINE);
        }

        /* Pfh-Probe STREAM */
        for (j = 0; j < 4; j ++) {
            avgtime[j] = 0;
            mintime[j] = UINT64_MAX;
            maxtime[j] = 0;
        }
        warmup(a, b, c, array_elements);
        for (j=0; j<array_elements; j++) {
            a[j] = 1.0;
            b[j] = 2.0;
            c[j] = 0.0;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        for (k=0; k<NTIMES; k++) {
            // kernel 1: Copy
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            vt_read(group, 1, 0, 0, 0);
            for (j=0; j<array_elements; j++)
                c[j] = a[j];
            vt_read(group, 2, 0, 0, 0);
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[0][k] = t1 - t0;

            // kernel 2: Scale
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            vt_read(group, 3, 0, 0, 0);
            for (j=0; j<array_elements; j++)
                b[j] = scalar*c[j];
            vt_read(group, 4, 0, 0, 0);
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[1][k] = t1-t0;
    
            // kernel 3: Add
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            vt_read(group, 5, 0, 0, 0);
            for (j=0; j<array_elements; j++)
                c[j] = a[j]+b[j];
            vt_read(group, 6, 0, 0, 0);
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[2][k] = t1-t0;
    
            // kernel 4: Triad
            _read_ns(t0, ts);
            MPI_Barrier(MPI_COMM_WORLD);
            vt_read(group, 7, 0, 0, 0);
            for (j=0; j<array_elements; j++)
                a[j] = b[j]+scalar*c[j];
            vt_read(group, 8, 0, 0, 0);
            MPI_Barrier(MPI_COMM_WORLD);
            _read_ns(t1, ts);
            times[3][k] = t1-t0;
        }
        MPI_Gather(times, 4*NTIMES, MPI_UINT64_T, TimesByRank, 4*NTIMES, MPI_UINT64_T, 0, MPI_COMM_WORLD);
        vt_write();
        if (myrank == 0) {
            for (k=0; k<NTIMES; k++) {
                for (j=0; j<4; j++) {
                    tmin = UINT64_MAX;
                    for (i=0; i<numranks; i++) {
                        // printf("DEBUG: Timing: iter %d, kernel %lu, rank %d, tmin %f, TbyRank %f\n",k,j,i,tmin,TimesByRank[4*NTIMES*i+j*NTIMES+k]);
                        tmin = MIN(tmin, TimesByRank[4*NTIMES*i+j*NTIMES+k]);
                    }
                    // printf("DEBUG: Final Timing: iter %d, kernel %lu, final tmin %f\n",k,j,tmin);
                    times[j][k] = tmin;
                }
            }

            for (k=0; k<NTIMES; k++) {
                for (j=0; j<4; j++) {
                    avgtime[j] = avgtime[j] + times[j][k];
                    mintime[j] = MIN(mintime[j], times[j][k]);
                    maxtime[j] = MAX(maxtime[j], times[j][k]);
                }
            }

            printf("Function    Best Rate B/ns  Avg time     Min time     Max time\n");
            for (j=0; j<4; j++) {
                avgtime[j] = avgtime[j]/NTIMES;

                printf("%s%11.6f  %llu  %llu  %llu\n", label[j],
                   bytes[j]/mintime[j],
                   avgtime[j],
                   mintime[j],
                   maxtime[j]);
            }
            printf(HLINE);
        }
        sprintf(myfile, "./data/%d_%llubytes_1.csv", myrank, cur_bytes);
        fp = fopen(myfile, "w");
        for (j = 0; j < NTIMES; j ++) {
            fprintf(fp, "%llu,%llu,%llu,%llu\n", times[0][j], times[1][j], times[2][j], times[3][j]);
        }
		fflush(fp);
		fclose(fp);
        /* --- Every Rank Checks its Results --- */
        computeSTREAMerrors(&AvgError[0], &AvgError[1], &AvgError[2]);
        /* --- Collect the Average Errors for Each Array on Rank 0 --- */
        MPI_Gather(AvgError, 3, MPI_DOUBLE, AvgErrByRank, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        /* -- Combined averaged errors and report on Rank 0 only --- */
        if (myrank == 0) {
            checkSTREAMresults(AvgErrByRank,numranks);
            printf(HLINE);
        }
        // Clean up
        free(a);
        free(b);
        free(c);

        // Increment kib
        if (cur_bytes < 1024) {
            cur_bytes += 64;
        } else if (cur_bytes < 1048576){
            cur_bytes += 1024;
        } else {
            cur_bytes += 1048576;
        }
    }


    if (myrank == 0) {
        free(TimesByRank);
        free(AvgErrByRank);
    }
    vt_clean();
    MPI_Finalize();
    return(0);
}

# define    M    20

int
checktick()
    {
    int        i, minDelta, Delta;
    double    t1, t2, timesfound[M];

/*  Collect a sequence of M unique time values from the system. */

    for (i = 0; i < M; i++) {
    t1 = MPI_Wtime();
    while( ((t2=MPI_Wtime()) - t1) < 1.0E-6 )
        ;
    timesfound[i] = t1 = t2;
    }

/*
 * Determine the minimum difference between these M values.
 * This result will be our estimate (in microseconds) for the
 * clock granularity.
 */

    minDelta = 1000000;
    for (i = 1; i < M; i++) {
    Delta = (int)( 1.0E6 * (timesfound[i]-timesfound[i-1]));
    minDelta = MIN(minDelta, MAX(Delta,0));
    }

   return(minDelta);
    }


// ----------------------------------------------------------------------------------
// For the MPI code I separate the computation of errors from the error
// reporting output functions (which are handled by MPI rank 0).
// ----------------------------------------------------------------------------------
#ifndef abs
#define abs(a) ((a) >= 0 ? (a) : -(a))
#endif
void computeSTREAMerrors(STREAM_TYPE *aAvgErr, STREAM_TYPE *bAvgErr, STREAM_TYPE *cAvgErr)
{
    STREAM_TYPE aj,bj,cj,scalar;
    STREAM_TYPE aSumErr,bSumErr,cSumErr;
    ssize_t    j;
    int    k;

    /* reproduce initialization */
    aj = 1.0;
    bj = 2.0;
    cj = 0.0;
    /* a[] is modified during timing check */
    // aj = 2.0E0 * aj;
    /* now execute timing loop */
    scalar = SCALAR;
    for (k=0; k<NTIMES; k++)
        {
            cj = aj;
            bj = scalar*cj;
            cj = aj+bj;
            aj = bj+scalar*cj;
        }

    /* accumulate deltas between observed and expected results */
    aSumErr = 0.0;
    bSumErr = 0.0;
    cSumErr = 0.0;
    for (j=0; j<array_elements; j++) {
        aSumErr += abs(a[j] - aj);
        bSumErr += abs(b[j] - bj);
        cSumErr += abs(c[j] - cj);
    }
    *aAvgErr = aSumErr / (STREAM_TYPE) array_elements;
    *bAvgErr = bSumErr / (STREAM_TYPE) array_elements;
    *cAvgErr = cSumErr / (STREAM_TYPE) array_elements;
}



void checkSTREAMresults (STREAM_TYPE *AvgErrByRank, int numranks)
{
    STREAM_TYPE aj,bj,cj,scalar;
    STREAM_TYPE aSumErr,bSumErr,cSumErr;
    STREAM_TYPE aAvgErr,bAvgErr,cAvgErr;
    double epsilon;
    ssize_t    j;
    int    k,ierr,err;

    // Repeat the computation of aj, bj, cj because I am lazy
    /* reproduce initialization */
    aj = 1.0;
    bj = 2.0;
    cj = 0.0;
    /* a[] is modified during timing check */
    // aj = 2.0E0 * aj;
    /* now execute timing loop */
    scalar = SCALAR;
    for (k=0; k<NTIMES; k++)
        {
            cj = aj;
            bj = scalar*cj;
            cj = aj+bj;
            aj = bj+scalar*cj;
        }

    // Compute the average of the average errors contributed by each MPI rank
    aSumErr = 0.0;
    bSumErr = 0.0;
    cSumErr = 0.0;
    for (k=0; k<numranks; k++) {
        aSumErr += AvgErrByRank[3*k + 0];
        bSumErr += AvgErrByRank[3*k + 1];
        cSumErr += AvgErrByRank[3*k + 2];
    }
    aAvgErr = aSumErr / (STREAM_TYPE) numranks;
    bAvgErr = bSumErr / (STREAM_TYPE) numranks;
    cAvgErr = cSumErr / (STREAM_TYPE) numranks;

    if (sizeof(STREAM_TYPE) == 4) {
        epsilon = 1.e-6;
    }
    else if (sizeof(STREAM_TYPE) == 8) {
        epsilon = 1.e-13;
    }
    else {
        printf("WEIRD: sizeof(STREAM_TYPE) = %lu\n",sizeof(STREAM_TYPE));
        epsilon = 1.e-6;
    }

    err = 0;
    if (abs(aAvgErr/aj) > epsilon) {
        err++;
        printf ("Failed Validation on array a[], AvgRelAbsErr > epsilon (%e)\n",epsilon);
        printf ("     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",aj,aAvgErr,abs(aAvgErr)/aj);
        ierr = 0;
        for (j=0; j<array_elements; j++) {
            if (abs(a[j]/aj-1.0) > epsilon) {
                ierr++;
#ifdef VERBOSE
                if (ierr < 10) {
                    printf("         array a: index: %ld, expected: %e, observed: %e, relative error: %e\n",
                        j,aj,a[j],abs((aj-a[j])/aAvgErr));
                }
#endif
            }
        }
        printf("     For array a[], %d errors were found.\n",ierr);
    }
    if (abs(bAvgErr/bj) > epsilon) {
        err++;
        printf ("Failed Validation on array b[], AvgRelAbsErr > epsilon (%e)\n",epsilon);
        printf ("     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",bj,bAvgErr,abs(bAvgErr)/bj);
        printf ("     AvgRelAbsErr > Epsilon (%e)\n",epsilon);
        ierr = 0;
        for (j=0; j<array_elements; j++) {
            if (abs(b[j]/bj-1.0) > epsilon) {
                ierr++;
#ifdef VERBOSE
                if (ierr < 10) {
                    printf("         array b: index: %ld, expected: %e, observed: %e, relative error: %e\n",
                        j,bj,b[j],abs((bj-b[j])/bAvgErr));
                }
#endif
            }
        }
        printf("     For array b[], %d errors were found.\n",ierr);
    }
    if (abs(cAvgErr/cj) > epsilon) {
        err++;
        printf ("Failed Validation on array c[], AvgRelAbsErr > epsilon (%e)\n",epsilon);
        printf ("     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",cj,cAvgErr,abs(cAvgErr)/cj);
        printf ("     AvgRelAbsErr > Epsilon (%e)\n",epsilon);
        ierr = 0;
        for (j=0; j<array_elements; j++) {
            if (abs(c[j]/cj-1.0) > epsilon) {
                ierr++;
#ifdef VERBOSE
                if (ierr < 10) {
                    printf("         array c: index: %ld, expected: %e, observed: %e, relative error: %e\n",
                        j,cj,c[j],abs((cj-c[j])/cAvgErr));
                }
#endif
            }
        }
        printf("     For array c[], %d errors were found.\n",ierr);
    }
    if (err == 0) {
        printf ("Solution Validates: avg error less than %e on all three arrays\n",epsilon);
    }
#ifdef VERBOSE
    printf ("Results Validation Verbose Results: \n");
    printf ("    Expected a(1), b(1), c(1): %f %f %f \n",aj,bj,cj);
    printf ("    Observed a(1), b(1), c(1): %f %f %f \n",a[1],b[1],c[1]);
    printf ("    Rel Errors on a, b, c:     %e %e %e \n",abs(aAvgErr/aj),abs(bAvgErr/bj),abs(cAvgErr/cj));
#endif
}
