#!/bin/bash

# === PerfHound Settings ===

include make.config

ARCH := $(shell uname -m)

# x86 PHASM links the ph_enable_pmu.ko user-space sysfs client
PH_PMU_OBJS =
ifeq ($(PH_API),PHASM)
ifeq ($(ARCH),x86_64)
PH_PMU_OBJS = ph_pmu_x86.o ph_evt_x86.o
endif
endif

# === Compiling settings, DO NOT TOUCH ===
CFLAGS = -O2 -fno-builtin -Wall -fPIC
CFLAGS += -DPH_OPT_$(PH_API) -DPH_OPT_$(PH_EVMODE)
CFLAGS += -DPH_OPT_BUFSIZE=$(PH_BUFSIZE)
CFLAGS += -I./include

ifeq ($(PH_PARMODE),SERIAL)
	PH_MAIN = perfhound
endif

ifeq ($(PH_PARMODE),MPI)
	CFLAGS += -DPH_OPT_MPI
	PH_MAIN = ph_mpi
endif

ifeq ($(PH_DEBUG),YES)
	CFLAGS += -DPH_DEBUG
endif

LDFLAGS = -fPIC
LDFLAGS_MPI = -fPIC
ifeq ($(ARCH),x86_64)
	LDFLAGS_MPI += -lmpi
endif

ifeq ($(PH_API),PAPI)
	PAPI_ROOT=$(PAPI)
	CFLAGS += -I$(PAPI_ROOT)/include
	LDFLAGS += -L$(PAPI_ROOT)/lib -lpapi
	LDFLAGS_MPI += -L$(PAPI_ROOT)/lib -lpapi
endif

# === TARGETS ===
.PHONY: all install clean

all: libph.so libphmpi.so

libph.so: perfhound.o ph_io.o $(PH_PMU_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o perfhound.o -c perfhound.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ph_io.o -c ph_io.c
ifneq ($(PH_PMU_OBJS),)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ph_pmu_x86.o -c ph_pmu_x86.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ph_evt_x86.o -c ph_evt_x86.c
endif
	$(CC) $(LDFLAGS) -DPH_OPT_TS -shared -o $@ perfhound.o ph_io.o $(PH_PMU_OBJS)
	rm -f *.o

libphmpi.so: ph_mpi.o ph_io.o mpi_op.o $(PH_PMU_OBJS)
	$(MPICC) $(CFLAGS) $(LDFLAGS) -o ph_mpi.o -c ph_mpi.c
	$(MPICC) $(CFLAGS) $(LDFLAGS) -o ph_io.o -c ph_io.c
	$(MPICC) $(CFLAGS) $(LDFLAGS) -o mpi_op.o -c mpi_op.c
ifneq ($(PH_PMU_OBJS),)
	$(MPICC) $(CFLAGS) $(LDFLAGS) -o ph_pmu_x86.o -c ph_pmu_x86.c
	$(MPICC) $(CFLAGS) $(LDFLAGS) -o ph_evt_x86.o -c ph_evt_x86.c
endif
	$(MPICC) $(LDFLAGS_MPI) -DPH_OPT_TS -shared -o $@ ph_mpi.o ph_io.o mpi_op.o $(PH_PMU_OBJS)
	rm -f *.o

install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	cp ./*.so ${PREFIX}/lib
	cp ./include/perfhound.h ${PREFIX}/include
	@echo PH-Probe has been installed in ${PREFIX}

clean:
	rm -rf *.o *.x *.so
