#!/bin/bash

# variables for test
mode=TS
tool=pfhd
nprocs=40
kernel=RDTSCP
measure_num=10000
resolution_test_num=30
interval_check_times=20
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


function cost_measure_test() {
    compile_and_run ${1} ${2} 0
    # get the measure cost
    python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f cost_measure -a ${cost_measure_file}
}

function interval_check() {
    target_round=${3} # round is the same as cydelay
    for j in `seq 2 $[interval_check_times + 1]`
    do
        compile_and_run ${1} ${2} $target_round $j
        python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f least_interval -a ${target_round},${cost_measure_file},${least_interval_file}
        passed=`cat ${least_interval_file}`
        if [[ ${passed} == "no" ]]; then
            break
        fi
    done
}

function least_interval_test() {
    i=0
    end=0
    start=0
    test_step=100000
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let i=i+test_step
            compile_and_run ${1} ${2} $i
            python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f least_interval -a ${i},${cost_measure_file},${least_interval_file}
            passed=`cat ${least_interval_file}`
            if [[ ${passed} == "no" ]]; then
                let start=i
            else
                interval_check ${1} ${2} $i
                check_passed=`cat ${least_interval_file}`
                if [[ ${check_passed} == "no" ]]; then
                    passed="no"
                    let start=i
                else
                    let end=i
                fi
            fi
        done
        let i=start
        let test_step=test_step/10
    done
}

function variation_resolution() {
    k=0
    left=0
    right=0
    test_step=10000
    resolution_file=${3}
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let k=k+test_step
            let end_round=least_round+k*resolution_test_num
            for i in `seq ${least_round} ${k} ${end_round}`
            do
                compile_and_run ${1} ${2} $i
            done
            python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f variation_resolution -a ${k},${least_round},${cost_measure_file},${resolution_file}
            passed=`cat ${resolution_file}`
            if [[ ${passed} == "no" ]]; then
                let left=k
            else
                let right=k
            fi
        done
        let k=left
        let test_step=test_step/10
    done
}

function main() {
    result_dir="result_files"
    if [ ! -d ${result_dir} ]; then
        mkdir -p ${result_dir}
    fi

    if [[ $nprocs -gt 20 ]]; then
        let factor=nprocs-20
    else
        let factor=nprocs
    fi
    let single_core_arrlen=7208960
    let arrlen=single_core_arrlen/factor

    # step-0: fix the frequency
    sudo cpupower frequency-set -g performance -u 2.5G -d 2.5G
    # step-1: measure cost test
    cost_measure_file="${result_dir}/${tool}_cost.txt"
    cost_measure_test ${nprocs} ${arrlen}
    cost_cycle=`sed -n '1p' ${cost_measure_file}`
    cost_nanosec=`sed -n '2p' ${cost_measure_file}`
    # step-2: least measurable interval
    least_interval_file="${result_dir}/${tool}_interval.txt"
    least_interval_test ${nprocs} ${arrlen}
    least_round=`sed -n '2p' ${least_interval_file}`
    least_interval_cycle=`sed -n '3p' ${least_interval_file}`
    least_interval_nanosec=`sed -n '4p' ${least_interval_file}`
    # # step-3: variation resolution
    cy_resolution_file="${result_dir}/${tool}_cycle_resolution.txt"
    ns_resolution_file="${result_dir}/${tool}_nanosec_resolution.txt"
    variation_resolution ${nprocs} ${arrlen} ${cy_resolution_file}
    variation_resolution ${nprocs} ${arrlen} ${ns_resolution_file}
    dcycle=`sed -n '2p' ${cy_resolution_file}`
    dnanosec=`sed -n '2p' ${ns_resolution_file}`
    # output result
    echo "[result-info] ${tool} cycle and nanosec measure cost: $cost_cycle $cost_nanosec"
    echo "[result-info] ${tool} cycle and nanosec least interval: $least_interval_cycle $least_interval_nanosec"
    echo "[result-info] ${tool} cycle and nanosec variation resolution: $dcycle $dnanosec"
}

main
