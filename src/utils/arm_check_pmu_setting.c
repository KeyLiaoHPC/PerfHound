#include <stdio.h>
#include <stdint.h>

void
read(uint64_t *val0, uint64_t *val1, uint64_t *val2) {
    asm volatile(
        "mrs %0, PMCR_EL0" "\n\t"
        "mrs %1, PMCCNTR_EL0" "\n\t"
        "mrs %2, PMCCFILTR_EL0" "\n\t"
        : "=r" (*val0), "=r" (*val1), "=r" (*val2)
        :
        :
    );

}

int
main(void) {
    uint64_t val0, val1, val2;
    read(&val0, &val1, &val2);
    printf("PMCR_EL0: 0x%x\n", val0);
    printf("PMCCNTR_EL0: 0x%x\n", val1);
    printf("PMCCFILTR_EL0: 0x%x\n", val2);

    return 0;
}
