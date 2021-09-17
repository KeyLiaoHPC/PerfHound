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


libpfh_ts.so: pfh_probe.c file_io.c 
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o pfh_probe.o -c pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(CC) $(LDFLAGS) -DPFH_MODE_TS -shared -o $@ pfh_probe.o  file_io.o 
	rm *.o
libpfh_evx.so: pfh_probe.c file_io.c 
	$(CC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o pfh_probe.o -c pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(CC) $(LDFLAGS) -DPFH_MODE_EVX -shared -o $@ pfh_probe.o  file_io.o 
	rm *.o
libpfhmpi_ts.so: mpi_op.c pfh_mpi.c file_io.c 
	$(MPICC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o pfh_mpi.o -c pfh_mpi.c
	$(MPICC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(MPICC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o mpi_op.o -c mpi_op.c
	$(MPICC) $(LDFLAGS) -DPFH_MODE_TS -shared -o $@ pfh_mpi.o mpi_op.o  file_io.o 
	rm *.o
libpfhmpi_evx.so: mpi_op.c pfh_mpi.c file_io.c 
	$(MPICC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o pfh_mpi.o -c pfh_mpi.c
	$(MPICC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(MPICC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o mpi_op.o -c mpi_op.c
	$(MPICC) $(LDFLAGS) -DPFH_MODE_EVX -shared -o $@ pfh_mpi.o mpi_op.o  file_io.o 
	rm *.o

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
	$(MPICC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o $@ -c $< 
libpfhmpi_evx.o: pfh_mpi.c
	$(MPICC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o $@ -c $< 

clean:
	rm -rf *.o *.x *.so
