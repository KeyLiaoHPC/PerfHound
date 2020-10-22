#!/bin/bash
MPICC = mpicc
CC = gcc
CFLAGS = -O2 -fno-builtin
CFLAGS += -I$(LIBPFC)/include
#CFLAGS += -DUSE_MPI
CFLAGS += -fPIC
# 
#VARAPI = /chpc/home/stf-keyliao-a/proj/Vartect/varapi
VARAPI = $(PWD)
VARAPI_FLAGS = -DUSE_VARAPI 
LDFLAGS =  -lpfc
# Installing path
PREFIX ?= .

VPATH = $(VARAPI)/include

.PHONY: all install clean

all: libvtapi_mpi_ts.so libvtapi_mpi_ev.so libvtapi_ts.so libvtapi_ev.so install

libvtapi_mpi_ts.so: varapi.mpi_ts.o file_op.mpi_ts.o vt_mpi.mpi_ts.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libvtapi_mpi_ev.so: varapi.mpi_ev.o file_op.mpi_ev.o vt_mpi.mpi_ev.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libvtapi_ts.so: varapi.ts.o file_op.ts.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

libvtapi_ev.so: varapi.ev.o file_op.ev.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	mv ./*.so ${PREFIX}/lib
	#mv ./*.x ${PREFIX}/bin
	cp ./*.h ${PREFIX}/include
	cp ./*.h ./include
	@echo VarAPI has been installed in ${PREFIX}

%.mpi_ts.o: %.c
	$(MPICC) $(CFLAGS) -DUSE_MPI $(STREAM_FLAGS) $(VARAPI_FLAGS) -DVT_MODE_TS -I$(VARAPI) $(LDFLAGS) -o $@ -c $< 

%.mpi_ev.o: %.c
	$(MPICC) $(CFLAGS) -DUSE_MPI $(STREAM_FLAGS) $(VARAPI_FLAGS) -DVT_MODE_EV -I$(VARAPI) $(LDFLAGS) -o $@ -c $< 

%.ts.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) -DVT_MODE_TS -I$(VARAPI) $(LDFLAGS) -o $@ -c $< 

%.ev.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) -DVT_MODE_EV -I$(VARAPI) $(LDFLAGS) -o $@ -c $< 

clean:
	rm -rf *.o *.x *.so
