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

.PHONY: all install clean

all: libpfh.so libpfh_evx.so libpfh_sys.so libpfh_sys_evx.so \
		libpfhmpi.so libpfhmpi_evx.so \
		libpfhmpi_sys.so libpfhmpi_sys_evx.so


libpfh.so: file_io.o 
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libpfh_evx.so: file_io.o 
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libpfh_sys.so: file_io.o 
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libpfh_sys_evx.so: file_io.o 
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libpfhmpi.so: file_io.o  mpi_op.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libpfhmpi_evx.so: file_io.o  mpi_op.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libpfhmpi_sys.so: file_io.o  mpi_op.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libpfhmpi_sys_evx.so: file_io.o  mpi_op.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^


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

libpfh.o: pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS  $(LDFLAGS) -o $@ -c $< 
libpfh_evx.o: pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_EVX  $(LDFLAGS) -o $@ -c $< 
libpfh_sys.o: pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS -DUSE_SYSCALL_TS  $(LDFLAGS) -o $@ -c $< 
libpfh_sys_evx.o: pfh_probe.c
	$(CC) $(CFLAGS) -DPFH_MODE_EVX -DUSE_SYSCALL_TS  $(LDFLAGS) -o $@ -c $< 

libpfhmpi.o: pfh_mpi.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS  $(LDFLAGS) -o $@ -c $< 
libpfhmpi_evx.o: pfh_mpi.c
	$(CC) $(CFLAGS) -DPFH_MODE_EVX  $(LDFLAGS) -o $@ -c $< 
libpfhmpi_sys.o: pfh_mpi.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS -DUSE_SYSCALL_TS  $(LDFLAGS) -o $@ -c $< 
libpfhmpi_sys_evx.o: pfh_mpi.c
	$(CC) $(CFLAGS) -DPFH_MODE_EVX -DUSE_SYSCALL_TS  $(LDFLAGS) -o $@ -c $< 

clean:
	rm -rf *.o *.x *.so
