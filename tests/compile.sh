#!/bin/bash -x
NINS=${NINS:-1000}
MODE=${MODE:-EVX}
NMEASURE=${NMEASURE:-1000}
TOOL=${TOOL:-PERFHOUND}

LIBPFIX=${MODE,,}

gcc -O2 -fno-builtin  -o test1_asm_x86.x -DMODE=$MODE \
	-DNINS=$NINS -DNMEASURE=$NMEASURE -DPERFHOUND -I../pfhprobe/include \
	-L../pfhprobe -lpfh_papi -L$PAPI/lib -lpapi\
	./test1_asm_x86.c

mpicc -O2 -fno-builtin  -o test3_asmmpi_x86.x -DMODE=$MODE \
	-DNINS=$NINS -DNMEASURE=$NMEASURE -DPERFHOUND -I../pfhprobe/include \
	-L../pfhprobe -lpfh_papi_mpi -L$PAPI/lib -lpapi\
	./test3_asmmpi_x86.c
