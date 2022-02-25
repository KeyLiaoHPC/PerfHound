#!/bin/sh

# test TS mode
mode=TS
arch=x86
measure_num=10050
source_file=../test_${arch}_mpi.c

for tool in PERFHOUND; do
    if [ $tool == PAPI ]; then
        link_lib="-lpfh_papi_mpi"
    else
        link_lib="-lpfh_mpi"
    fi
    for kernel in ADD; do
        for method in CONTINUE NON_CONTINUE; do
            mkdir -p $method 
            cd $method
            sudo cpupower frequency-set -g performance -u 2.6G -d 2.6G
            exe=../${tool}_${mode}_${kernel}_${method}_mpi.x
            flags="-O2 -std=gnu99 -DKNAME=$kernel -DMODE=$mode -D$tool -DNMEASURE=$measure_num -D$method"
            for ((i=1000;i<=1000;i+=1000)); do
                echo "mode=${mode}, tool=${tool}, kernel=${kernel}, method=${method}, NINS=${i}"
                mpicc ${flags} -DNINS=$i -o $exe $source_file $link_lib
                # mpirun -np 40 --mca pml ob1 --mca btl ^openib --bind-to core ./${exe}
                mpirun -np 40 --mca btl ^openib --bind-to core ./${exe}
                rm $exe
            done
            cd ..
        done
    done
done
