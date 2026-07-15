# Out-of-tree kbuild wrapper; obj-m on the KERNELRELEASE pass (M=, not SUBDIRS=).
ifneq ($(KERNELRELEASE),)
obj-m := ph_enable_pmu.o
ph_enable_pmu-y := enable_pmu_x86.o
else

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

.PHONY: all clean

all:
	@test -d "$(KDIR)" || (echo "*** [PH-kmod] Missing kernel headers: $(KDIR)"; exit 1)
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif
