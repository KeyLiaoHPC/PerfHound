#!/bin/bash

# === PerfHound Settings ===

include make.config

# === Compiling settings, DO NOT TOUCH ===
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
BUF ?= 4
PFHFLAGS=-DPFH_RECBUF_KIB=$(BUF)

.PHONY: all install clean

all: libperfhound.so


libperfhound.so: perfhound.c file_io.c 
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o perfhound.o -c perfhound.c
	$(CC) $(CFLAGS) -DPFH_MODE_TS $(PFHFLAGS) $(LDFLAGS) -o pfh_io.o -c pfh_io.c
	$(CC) $(LDFLAGS) -DPFH_MODE_TS -shared -o $@ pfh_probe.o pfh_io.o 
	rm *.o


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
