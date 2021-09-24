#!/bin/bash
PFHPROBE = $(PWD)
MPICC = mpicc
CC = gcc
CFLAGS = -O2 -fno-builtin -Wall -fPIC

CFLAGS += -I$(PFHPROBE)/include -I$(MPI)/include
LDFLAGS =  -fPIC
MPILDFLAGS = -L$(MPI)/lib -lmpi
ARCH := $(shell uname -p)
ifeq ($(ARCH),x86_64)
    CFLAGS += -I$(LIBPFC)/include 
    LDFLAGS += -L$(LIBPFC)/lib64 -lpfc
endif

PAPI_ROOT=$(PAPI)
PAPI_INC=-I$(PAPI_ROOT)/include
PAPI_LIB=-L$(PAPI_ROOT)/lib -lpapi


#CFLAGS += -DUSE_MPI
CFLAGS += -fPIC
# Installing path
PREFIX ?= .
BUF=4
PFHFLAGS=-DPFH_RECBUF_KIB=$(BUF)

.PHONY: all install clean

all: libpfh.so libpfh_mpi.so libpfh_papi.so libpfh_papi_mpi.so


libpfh.so: pfh_probe.c file_io.c 
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o pfh_probe.o -c pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(CC) $(LDFLAGS) -DPFH_MODE_TS -shared -o $@ pfh_probe.o  file_io.o 
	rm *.o
libpfh_mpi.so: pfh_probe.c file_io.c 
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o pfh_probe.o -c pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(CC) $(LDFLAGS) -DPFH_MODE_TS -shared -o $@ pfh_probe.o  file_io.o 
	rm *.o
#libpfh_evx.so: pfh_probe.c file_io.c 
#	$(CC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o pfh_probe.o -c pfh_probe.c
#	$(CC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
#	$(CC) $(LDFLAGS) -DPFH_MODE_EVX -shared -o $@ pfh_probe.o  file_io.o 
#	rm *.o
libpfh_papi.so: pfh_probe.c file_io.c 
	$(CC) $(CFLAGS) $(PAPI_INC) -DPFH_MODE_TS -DUSE_PAPI $(PFHFLAGS) $(LDFLAGS) -o pfh_probe.o -c pfh_probe.c
	$(CC) $(CFLAGS) $(PAPI_INC) -DPFH_MODE_TS -DUSE_PAPI $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(CC) $(LDFLAGS) $(PAPI_INC) -DPFH_MODE_TS -DUSE_PAPI -shared -o $@ pfh_probe.o  file_io.o  $(PAPI_LIB)
	rm *.o
#libpfhmpi_evx.so: mpi_op.c pfh_mpi.c file_io.c 
#	$(MPICC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o pfh_mpi.o -c pfh_mpi.c
#	$(MPICC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
#	$(MPICC) $(CFLAGS) -DPFH_MODE_EVX $(PFHFLAGS) $(LDFLAGS) -o mpi_op.o -c mpi_op.c
#	$(MPICC) $(LDFLAGS) -DPFH_MODE_EVX -shared -o $@ pfh_mpi.o mpi_op.o  file_io.o 
#	rm *.o
libpfh_papi_mpi.so: mpi_op.c pfh_mpi.c file_io.c 
	$(MPICC) $(CFLAGS) $(PAPI_INC) -DPFH_MODE_TS -DUSE_PAPI $(PFHFLAGS) $(LDFLAGS) -o pfh_mpi.o -c pfh_mpi.c
	$(MPICC) $(CFLAGS) $(PAPI_INC) -DPFH_MODE_TS -DUSE_PAPI $(PFHFLAGS) $(LDFLAGS) -o file_io.o -c file_io.c
	$(MPICC) $(CFLAGS) $(PAPI_INC) -DPFH_MODE_TS -DUSE_PAPI $(PFHFLAGS) $(LDFLAGS) -o mpi_op.o -c mpi_op.c
	$(MPICC) $(LDFLAGS) $(PAPI_INC) -DPFH_MODE_TS -DUSE_PAPI -shared -o $@ pfh_mpi.o mpi_op.o  file_io.o  $(PAPI_LIB)
	rm *.o


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

install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	cp ./*.so ${PREFIX}/lib
	cp ./include/pfhprobe.h ./include/pfh_mpi.h ${PREFIX}/include
	rm *.o
	@echo Pfh-Probe has been installed in ${PREFIX}


clean:
	rm -rf *.o *.x *.so
