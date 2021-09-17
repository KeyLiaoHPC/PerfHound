#!/bin/bash
PFHPROBE = $(PWD)
MPICC = mpicc
CC = gcc
CFLAGS = -O2 -fno-builtin -Wall

CFLAGS += -I$(PFHPROBE)/include -I$(MPI)/include
LDFLAGS =  
MPILDFLAGS = -L$(MPI)/lib -lmpi
ARCH := $(shell uname -p)
ifeq ($(ARCH),x86_64)
    CFLAGS += -I$(LIBPFC)/include 
    LDFLAGS += -L$(LIBPFC)/lib64 -lpfc
endif

#CFLAGS += -DUSE_MPI
CFLAGS += -fPIC
# Installing path
PREFIX ?= .
BUF=4
PFHFLAGS=-DPFH_RECBUF_KIB=$(BUF)

.PHONY: all install clean

all: libpfh_ts.so libpfh_evx.so libpfhmpi_ts.so libpfhmpi_evx.so


libpfh_ts.so: libpfh_ts.o file_io.o 
	$(CC) $(LDFLAGS) -shared -o $@ $^
libpfh_evx.so: libpfh_evx.o file_io.o 
	$(CC) $(LDFLAGS) -shared -o $@ $^

libpfhmpi_ts.so: libpfhmpi_ts.o file_io.o  mpi_op.o
	$(MPICC) $(LDFLAGS) -shared -o $@ $^ 
libpfhmpi_evx.so: libpfhmpi_evx.o  file_io.o  mpi_op.o
	$(MPICC) $(LDFLAGS) -shared -o $@ $^


install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	cp ./*.so ${PREFIX}/lib
	cp ./include/pfhprobe.h ./include/pfh_mpi.h ${PREFIX}/include
	rm *.o
	@echo Pfh-Probe has been installed in ${PREFIX}

file_io.o: file_io.c 
	$(CC) $(CFLAGS)  $(LDFLAGS) -o $@ -c $< 
mpi_op.o: mpi_op.c
	$(MPICC) $(CFLAGS)  $(LDFLAGS) -o $@ -c $< 

libpfh_ts.o: pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o $@ -c $< 
libpfh_evx.o: pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o $@ -c $< 

libpfhmpi_ts.o: pfh_mpi.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o $@ -c $< 
libpfhmpi_evx.o: pfh_mpi.c
	$(CC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o $@ -c $< 

clean:
	rm -rf *.o *.x *.so
