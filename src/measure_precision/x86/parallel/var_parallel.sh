#!/bin/bash

# variables for test
mode=TS
tool=papi
kernel=ADD
measure_num=10000
src_file=test_x86_parallel.c
total_cores=`grep 'processor' /proc/cpuinfo | wc -l`


function compile_and_run() {
    nprocs=${1}
    arrlen=${2}
    cydelay=${3}
    if [ ! ${4} ]; then
        run_id="1"
    else
        run_id=${4}
    fi
    output_dir="./data/${tool}/nprocs_${nprocs}/arrlen_${arrlen}/${mode}_${kernel}_test_6248/cydelay_${cydelay}/run_${run_id}"
    if [ ! -d ${output_dir} ]; then
        exe=./${tool}_${mode}_${kernel}_${cydelay}_mpi.x
        flags="-O3 -std=gnu99 -DTOOL=$tool -DMODE=$mode -DKNAME=$kernel -DCYDELAY=$cydelay -DARRLEN=$arrlen -DNMEASURE=$measure_num"
        echo "tool=${tool}, mode=${mode}, kernel=${kernel}, cycle_delay=${cydelay}, arrlen=${arrlen}"
        mpicc ${flags} -o $exe $src_file -lperfhound_mpi
        mpirun -np ${nprocs} --mca btl ^openib --bind-to core --map-by slot:pe=1 ${exe}
        rm ${exe}
    fi
}

function mytest() {
    compile_and_run ${1} ${2} ${3}
    python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f cost_measure -a ${cost_measure_file}
}

function main() {
    result_dir="var_parallel"
    if [ ! -d ${result_dir} ]; then
        mkdir -p ${result_dir}
    fi

    # step-0: fix the frequency
    sudo cpupower frequency-set -g performance -u 2.5G -d 2.5G
    for core_num in 1 2 4 8 10 16 20 40
    do
        if [[ $core_num -gt 20 ]]; then
            let factor=core_num-20
        else
            let factor=core_num
        fi
        let single_core_arrlen=7208960
        let len=single_core_arrlen/factor
        for cy in 500
        do
            cost_measure_file="${result_dir}/${tool}_cost_nprocs_${core_num}_arrlen_${len}_cydelay_${cy}.txt"
            mytest $core_num $len $cy
        done
    done
    mv data ${result_dir}
    rm ${result_dir}/*.txt
}

main
