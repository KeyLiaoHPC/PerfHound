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
VARAPI_FLAGS = -DUSE_VARAPI -DVT_MODE_EV #-DVT_MODE_SHORT -DVT_UEV_1
LDFLAGS = -lpfc
# Installing path
PREFIX ?= .

VPATH = $(VARAPI)/include

.PHONY: all install clean

all: libvarapi_mpi.so libvarapi.so install

libvarapi_mpi.so: varapi.mpi.o file_op.mpi.o vt_mpi.mpi.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libvarapi.so: varapi.o file_op.o
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

%.mpi.o: %.c
	$(MPICC) $(CFLAGS) -DUSE_MPI $(STREAM_FLAGS) $(VARAPI_FLAGS) -I$(VARAPI) $(LDFLAGS) -o $@ -c $< 

%.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) -I$(VARAPI) $(LDFLAGS) -o $@ -c $< 

clean:
	rm -rf *.o *.x *.so
