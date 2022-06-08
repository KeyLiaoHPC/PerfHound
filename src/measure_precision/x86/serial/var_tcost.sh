#!/bin/bash

# some variables
mode=TS
tool=papi
kernel=ADD
bind_core=12
arrlen=0
measure_num=10000
src_file=test_x86_serial.c

function compile_and_run() {
    cydelay=${1}
    if [ ! ${2} ]; then
        run_id="1"
    else
        run_id=${2}
    fi
    output_dir="./data/${tool}/arrlen_${arrlen}/${mode}_${kernel}_test_6248/cydelay_${cydelay}/run_${run_id}"
    if [ ! -d ${output_dir} ]; then
        exe=./${tool}_${mode}_${kernel}_${cydelay}.x
        flags="-O3 -std=gnu99 -DTOOL=$tool -DMODE=$mode -DKNAME=$kernel -DCYDELAY=$cydelay -DARRLEN=$arrlen -DNMEASURE=$measure_num"
        echo "tool=${tool}, mode=${mode}, kernel=${kernel}, cycle_delay=${cydelay}, arrlen=${arrlen}"
        gcc ${flags} -o $exe $src_file -lperfhound
        taskset -c ${bind_core} ${exe}
        rm ${exe}
    fi
}

function mytest() {
    compile_and_run ${1} ${2}
    python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f cost_measure -a ${cost_measure_file}
}

function main() {
    result_dir="var_tcost"
    if [ ! -d ${result_dir} ]; then
        mkdir -p ${result_dir}
    fi

    # fix the frequency
    sudo cpupower frequency-set -g performance -u 2.5G -d 2.5G

    for i in `seq 100`
    do
        cy=0
        cost_measure_file="${result_dir}/${tool}_cost_arrlen_${arrlen}_cydelay_${cy}_${i}.txt"
        mytest $cy $i
    done
    mv data ${result_dir}
    rm ${result_dir}/*.txt
}

main
