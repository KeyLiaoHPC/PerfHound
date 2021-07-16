#!/bin/bash
for i in {1..200000}
do
    echo '============ # of ADD = $i ============='
    make NADD=$i NMEASURE=10000 PROF=PERFHOUND
    taskset -c 11 ./test1_3regs_add_PERFHOUND.x
    make clean
    make NADD=$i NMEASURE=10000 PROF=PAPI
    taskset -c 11 ./test1_3regs_add_PAPI.x
    mv papi_out.txt ./papi_log/papi_out_${i}.txt
    make clean
done
