# Out-of-tree kbuild wrapper (same pattern as legacy armv8a/enable_pmu).
# obj-m must live in this file so kbuild picks it up on the KERNELRELEASE pass.
obj-m := ph_enable_pmu.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

ifneq ($(KERNELRELEASE),)
# kbuild pass: rules above only
else

.PHONY: all clean

all:
	@test -d "$(KDIR)" || (echo "*** [PH-kmod] Missing kernel headers: $(KDIR)"; exit 1)
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif
