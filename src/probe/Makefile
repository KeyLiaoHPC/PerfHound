#!/bin/bash

# === PerfHound Settings ===

include make.config

# === Compiling settings, DO NOT TOUCH ===
CFLAGS = -O2 -fno-builtin -Wall -fPIC
CFLAGS += -DPFH_OPT_$(PFH_API) -DPFH_OPT_$(PFH_EVMODE)
CFLAGS += -DPFH_OPT_BUFSIZE=$(PFH_BUFSIZE)
CFLAGS += -I./include 

ifeq ($(PFH_PARMODE),MPI)
	CFLAGS += -DPFH_OPT_MPI
endif

ifeq ($(PFH_DEBUG),YES)
	CFLAGS += -DPFH_DEBUG
endif


LDFLAGS =  -fPIC

ifeq ($(ARCH),x86_64)
	LDFLAGS += -lmpi
endif

ARCH := $(shell uname -p)
ifeq ($(ARCH),x86_64)
    CFLAGS += -I$(LIBPFC)/include 
    LDFLAGS += -L$(LIBPFC)/lib64 -lpfc
endif

ifeq ($(PFH_API),PAPI)
	PAPI_ROOT=$(PAPI)
	CFLAGS += -I$(PAPI_ROOT)/include
	LD_FLAGS += -L$(PAPI_ROOT)/lib -lpapi
endif


# === TARGETS ===
.PHONY: all install clean

all: libperfhound.so

libperfhound.so: perfhound.c pfh_io.c 
	$(CC) $(CFLAGS)  $(LDFLAGS) -o perfhound.o -c perfhound.c
	$(CC) $(CFLAGS)  $(LDFLAGS) -o pfh_io.o -c pfh_io.c
	$(CC) $(LDFLAGS) -DPFH_OPT_TS -shared -o $@ perfhound.o pfh_io.o 
	rm *.o

install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	cp ./*.so ${PREFIX}/lib
	cp ./include/perfhound.h ${PREFIX}/include
	@echo Pfh-Probe has been installed in ${PREFIX}

clean:
	rm -rf *.o *.x *.so
