#!/bin/bash -x
#NINS=${NINS:-1000}
NINS=$1
MODE=${MODE:-TS}
KNAME=${KNAME:-ST}
NMEASURE=${NMEASURE:-1050}
TOOL=${TOOL:-PERFHOUND}

LIBPFIX=${MODE,,}

#gcc -O2 -fno-builtin  -o test1_asm_x86.x -DMODE=$MODE \
#    -DNINS=$NINS -DNMEASURE=$NMEASURE -DPERFHOUND -I../pfhprobe/include \
#    -L../pfhprobe -lpfh    \
#    ./test1_asm_x86.c

mpicc -O2 -fno-builtin  -o test3_papi_${NINS}.x -DPAPI -DMODE=$MODE -DKNAME=$KNAME \
    -DNINS=$NINS -DNMEASURE=$NMEASURE -I../pfhprobe/include \
    -L../pfhprobe -lpfh_papi_mpi  -L$PAPI -lpapi\
    ./test3_asmmpi_x86.c
mpicc -O2 -fno-builtin  -o test3_pfh_${NINS}.x  -DMODE=$MODE -DKNAME=$KNAME \
    -DNINS=$NINS -DNMEASURE=$NMEASURE  -I../pfhprobe/include \
    -L../pfhprobe -lpfh_mpi  \
    ./test3_asmmpi_x86.c
#mpicc -O2 -fno-builtin  -o test3_asmmpi_x86.x -DMODE=$MODE \
#    -DNINS=$NINS -DNMEASURE=$NMEASURE \
#    ./test3_asmmpi_x86.c
