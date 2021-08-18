#!/bin/bash
PFHPROBE = $(PWD)
MPICC = mpicc
CC = gcc
CFLAGS = -O2 -fno-builtin -Wall

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
PFHPROBE_FLAGS = 
# Installing path
PREFIX ?= .

.PHONY: all install clean

all: libpfh.so 


libpfh.so: file_io.o pfh_probe.o
	$(LINK.c) $(LDFLAGS) -shared -o $@ $^

install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	cp ./*.so ${PREFIX}/lib
	cp ./include/pfhprobe.h ${PREFIX}/include
	rm *.o
	@echo Pfh-Probe has been installed in ${PREFIX}

%.o: %.c
	$(MPICC) $(CFLAGS) $(STREAM_FLAGS) $(PHPROBE_FLAGS) -DPFH_MODE_TS  $(LDFLAGS) -o $@ -c $< 

clean:
	rm -rf *.o *.x *.so
