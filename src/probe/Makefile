#!/bin/bash

# === PerfHound Settings ===

include make.config

# === Compiling settings, DO NOT TOUCH ===
LDFLAGS =  -fPIC
CFLAGS = -O2 -fno-builtin -Wall -fPIC
CFLAGS += -DPFH_OPT_$(PFH_API) -DPFH_OPT_$(PFH_EVMODE)
CFLAGS += -DPFH_OPT_BUFSIZE=$(PFH_BUFSIZE)
CFLAGS += -I./include 

ifeq ($(PFH_PARMODE),MPI)
	CFLAGS += -DPFH_OPT_MPI
	INCLUDE_FILE = ./include/perfhound_mpi.h
	TARGET_LIB = libperfhound_mpi.so
else
	INCLUDE_FILE = ./include/perfhound.h
	TARGET_LIB = libperfhound.so
endif

ifeq ($(PFH_DEBUG),YES)
	CFLAGS += -DPFH_DEBUG
endif

ARCH := $(shell uname -p)
ifeq ($(ARCH),x86_64)
	CFLAGS += -I$(LIBPFC)/include
	LDFLAGS += -L$(LIBPFC)/lib64 -lpfc
endif

ifeq ($(PFH_API),PAPI)
	PAPI_ROOT=$(PAPI)
	CFLAGS += -I$(PAPI_ROOT)/include
	LDFLAGS += -L$(PAPI_ROOT)/lib -lpapi
endif

# === TARGETS ===
.PHONY: all install clean

all: $(TARGET_LIB)

libperfhound.so: pfh_io.c perfhound.c
	$(CC) $(CFLAGS)  $(LDFLAGS) -o pfh_io.o -c pfh_io.c
	$(CC) $(CFLAGS)  $(LDFLAGS) -o perfhound.o -c perfhound.c
	$(CC) $(LDFLAGS) -DPFH_OPT_$(PFH_EVMODE) -shared -o $@ pfh_io.o perfhound.o
	rm *.o

libperfhound_mpi.so: pfh_io.c mpi_op.c perfhound_mpi.c
	$(CC) $(CFLAGS)  $(LDFLAGS) -o pfh_io.o -c pfh_io.c
	$(CC) $(CFLAGS)  $(LDFLAGS) -o mpi_op.o -c mpi_op.c
	$(CC) $(CFLAGS)  $(LDFLAGS) -o perfhound_mpi.o -c perfhound_mpi.c
	$(CC) $(LDFLAGS) -DPFH_OPT_$(PFH_EVMODE) -shared -o $@ pfh_io.o mpi_op.o perfhound_mpi.o
	rm *.o

install:
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	mkdir -p $(PREFIX)/bin
	cp ./*.so $(PREFIX)/lib
	cp $(INCLUDE_FILE) $(PREFIX)/include
	@echo PerfHound has been installed in $(PREFIX)

clean:
	rm -rf *.o *.x *.so
