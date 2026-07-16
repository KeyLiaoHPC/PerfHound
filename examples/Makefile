#!/usr/bin/make -f
# PerfHound examples build (current ph_* / phmpi_* API)

PH_ROOT   ?= ../src/probe
ARCH      := $(shell uname -m)
CC        ?= gcc
MPICC     ?= mpicc

ifeq ($(ARCH),aarch64)
PH_API    ?= PHASM
else
PH_API    ?= PAPI
endif

MODE      ?= EV
NINS      ?= 100
NMEASURE  ?= 205
NTESTS    ?= 200
NP        ?= 16
KNAME     ?= ADD
ARRLEN    ?= 1000
STREAM_ARRAY_SIZE ?= 2000000
NTIMES    ?= 10

PH_INC    := -I$(PH_ROOT)/include -I.
PH_LIB    := -L$(PH_ROOT)/lib
PH_RPATH  := -Wl,-rpath,$(abspath $(PH_ROOT)/lib)
CFLAGS    := -O2 -fno-builtin -DPERFHOUND -DMODE=$(MODE) \
             -DNINS=$(NINS) -DNMEASURE=$(NMEASURE) -DNTESTS=$(NTESTS) \
             -DKNAME=$(KNAME) $(PH_INC)
ifeq ($(PH_API),PAPI)
CFLAGS += -DPH_API_PAPI
endif
MPI_CFLAGS := $(CFLAGS) -DARRLEN=$(ARRLEN)
LDFLAGS   := $(PH_LIB) $(PH_RPATH)

.PHONY: all all-x86 clean probe run-all run-all-arm run-test1 run-test2 run-test3 run-test4 run-test5 run-test6 run-stream

all: probe all-x86

probe:
	$(MAKE) -C $(PH_ROOT) clean all install PH_API=$(PH_API) MPICC="$(MPICC)" CC=$(CC)

ifeq ($(ARCH),x86_64)
all-x86: test1_asm_x86.x test3_asmmpi_x86.x test5_vardist_add.x test6.x stream_mpi.x
else ifeq ($(ARCH),aarch64)
all-x86: test2_asm_armv8.x test4_asmmpi_armv8.x test5_vardist_add.x stream_mpi.x
else
all-x86:
	@echo "Unsupported architecture: $(ARCH)"
	@exit 1
endif

test1_asm_x86.x: test1_asm_x86.c ph_events.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lph

test2_asm_armv8.x: test2_asm_armv8.c ph_events.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lph

test3_asmmpi_x86.x: test3_asmmpi_x86.c ph_events.h
	$(MPICC) $(MPI_CFLAGS) -o $@ $< $(LDFLAGS) -lphmpi

test4_asmmpi_armv8.x: test4_asmmpi_armv8.c ph_events.h
	$(MPICC) $(MPI_CFLAGS) -o $@ $< $(LDFLAGS) -lphmpi

test5_vardist_add.x: test5_vardist_add.c ph_events.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lph -lm

test6.x: test6.c ph_events.h
	$(MPICC) $(MPI_CFLAGS) -o $@ $< $(LDFLAGS) -lphmpi

stream_mpi.x: stream_mpi.c ph_events.h
	$(MPICC) $(MPI_CFLAGS) -DSTREAM_ARRAY_SIZE=$(STREAM_ARRAY_SIZE) -DNTIMES=$(NTIMES) \
		-o $@ $< $(LDFLAGS) -lphmpi -lm

clean:
	rm -f *.x test5_rand_list.txt
	rm -rf ph_data

run-test1: test1_asm_x86.x
	./run_serial.sh test1_asm_x86.x

run-test3: test3_asmmpi_x86.x
	NP=$(NP) ./run_mpi.sh test3_asmmpi_x86.x

run-test2: test2_asm_armv8.x
	./run_serial.sh test2_asm_armv8.x

run-test4: test4_asmmpi_armv8.x
	NP=$(NP) ./run_mpi.sh test4_asmmpi_armv8.x

run-test5: test5_vardist_add.x
	./run_serial.sh test5_vardist_add.x

run-stream: stream_mpi.x
	NP=$(NP) ./run_mpi.sh stream_mpi.x

run-test6: test6.x
	NP=$(NP) ./run_mpi.sh test6.x

run-all: run-test1 run-test3 run-test5 run-test6 run-stream

run-all-arm: run-test2 run-test4 run-test5 run-stream
