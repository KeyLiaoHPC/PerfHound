#!/bin/bash
CC = mpicc
CFLAGS = -O3 -fno-builtin
#CFLAGS += -DUSE_MPI
CFLAGS += -fPIC
# 
#VARAPI = /chpc/home/stf-keyliao-a/proj/Vartect/varapi
VARAPI = $(PWD)
VARAPI_FLAGS = -DUSE_VARAPI #-DVT_MODE_SHORT -DVT_UEV_1
LDFLAGS = 
# Installing path
PREFIX ?= .

VPATH = $(VARAPI)/include

.PHONY: all install clean

all: libvartect_par.so libvartect_ser.so install

libvartect_par.so: varapi.par.o file_op.par.o vt_mpi.par.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^
libvartect_ser.so: varapi.ser.o file_op.ser.o
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

%.par.o: %.c
	$(CC) $(CFLAGS) -DUSE_MPI $(STREAM_FLAGS) $(VARAPI_FLAGS) -I$(VARAPI) -o $@ -c $< 

%.ser.o: %.c
	$(CC) $(CFLAGS) $(STREAM_FLAGS) $(VARAPI_FLAGS) -I$(VARAPI) -o $@ -c $< 

clean:
	rm -rf *.o *.x *.so
