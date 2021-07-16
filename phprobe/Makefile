#!/bin/bash
PFHPROBE = $(PWD)
MPICC = mpicc
CC = gcc
CFLAGS = -O2 -fno-builtin

CFLAGS += -I$(PFHPROBE)/include
LDFLAGS =  
ARCH := $(shell uname -p)
ifeq ($(ARCH),x86_64)
    CFLAGS += -I$(LIBPFC)/include 
    LDFLAGS += -L$(LIBPFC)/lib64 -lpfc
endif

#CFLAGS += -DUSE_MPI
CFLAGS += -fPIC
# 
#VARAPI = /chpc/home/stf-keyliao-a/proj/Vartect/varapi
PHPROBE_FLAGS = -DUSE_VARAPI
# Installing path
PREFIX ?= .

VPATH = $(VARAPI)/include

.PHONY: all install clean

all: libph-ts-mpi.so libph-ev-mpi.so libph-evx-mpi.so \
     libph-ts.so libph-ev.so libph-evx.so install


libph-ts-mpi.so: varapi.mpi_ts.o file_op.mpi_ts.o vt_mpi.mpi_ts.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libph-ev-mpi.so: varapi.mpi_ev.o file_op.mpi_ev.o vt_mpi.mpi_ev.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libph-evx-mpi.so: varapi.mpi_evx.o file_op.mpi_ev.o vt_mpi.mpi_ev.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libph-ts.so: varapi.ts.o file_op.ts.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libph-ev.so: varapi.ev.o file_op.ev.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libph-evx.so: varapi.evx.o file_op.ev.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	cp ./*.so ${PREFIX}/lib
	cp ./include/varapi.h ${PREFIX}/include
	rm *.o
	@echo VarAPI has been installed in ${PREFIX}

%.mpi_ts.o: %.c
	$(MPICC) $(CFLAGS) -DUSE_MPI $(STREAM_FLAGS) $(PHPROBE_FLAGS) -DVT_MODE_TS  $(LDFLAGS) -o $@ -c $< 

%.mpi_ev.o: %.c
	$(MPICC) $(CFLAGS) -DUSE_MPI $(STREAM_FLAGS) $(PHPROBE_FLAGS) -DVT_MODE_EV  $(LDFLAGS) -o $@ -c $< 

%.mpi_evx.o: %.c
	$(MPICC) $(CFLAGS) -DUSE_MPI $(STREAM_FLAGS) $(PHPROBE_FLAGS) -DVT_MODE_EVX  $(LDFLAGS) -o $@ -c $< 

%.ts.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(PHPROBE_FLAGS) -DVT_MODE_TS $(LDFLAGS) -o $@ -c $< 

%.ev.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(PHPROBE_FLAGS) -DVT_MODE_EV $(LDFLAGS) -o $@ -c $< 

%.evx.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(PHPROBE_FLAGS) -DVT_MODE_EVX $(LDFLAGS) -o $@ -c $<

clean:
	rm -rf *.o *.x *.so
