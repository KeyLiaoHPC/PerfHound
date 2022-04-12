#!/bin/bash

# variables for test
mode=TS
kernel=ADD
tool=PERFHOUND
arrlen=720896
measure_num=10000
check_times=20
src_file=test_x86_parallel.c


function compile_and_run() {
    round=${1}
    if [ ! ${2} ]; then
        run_id="1"
    else
        run_id=${2}
    fi
    output_dir="./data/${backend}/arrlen_${arrlen}/${mode}_${kernel}_test_6248_20220222/round_${round}/run_${run_id}"
    if [ ! -d ${output_dir} ]; then
        exe=./${tool}_${mode}_${kernel}_${round}_mpi.x
        flags="-O3 -std=gnu99 -D$tool -DMODE=$mode -DKNAME=$kernel -DROUND=$round -DARRLEN=$arrlen -DNMEASURE=$measure_num"
        echo "tool=${tool}, mode=${mode}, kernel=${kernel}, round=${round}, arrlen=${arrlen}"
        mpicc ${flags} -o $exe $src_file $link_lib
        mpirun -np 40 --mca btl ^openib --bind-to core --map-by slot:pe=1 ${exe}
        rm ${exe}
    fi
}


function cost_measure_test() {
    compile_and_run 0
    # get the measure cost
    python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f cost_measure -a ${cost_measure_file}
}

function interval_check() {
    target_round=${1}
    for j in `seq 2 $[check_times + 1]`
    do
        compile_and_run $target_round $j
        python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f least_interval -a ${target_round},${cost_measure_file},${least_interval_file}
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
            compile_and_run $i
            python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f least_interval -a ${i},${cost_measure_file},${least_interval_file}
            passed=`cat ${least_interval_file}`
            if [[ ${passed} == "no" ]]; then
                let start=i
            else
                interval_check $i
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


function main() {
    if [ ${tool} == PAPI ]; then
        link_lib="-lpfh_papi_mpi"
        backend="papi"
    else
        link_lib="-lpfh_mpi"
        backend="pfhd"
    fi

    result_dir="test_results"
    if [ ! -d ${result_dir} ]; then
        mkdir -p ${result_dir}
    fi

    # get the number of total cores
    total_cores=`grep 'processor' /proc/cpuinfo | wc -l`
    # loop for all cores
    for core_id in `seq 0 $[total_cores - 1]`
    do
        # step-0: fix the frequency
        sudo cpupower frequency-set -g performance -u 2.6G -d 2.6G
        # step-1: measure cost test
        cost_measure_file="${result_dir}/cost_${core_id}.txt"
        cost_measure_test
        cost_cycle=`sed -n '1p' ${cost_measure_file}`
        cost_nanosec=`sed -n '2p' ${cost_measure_file}`
        echo "[result-info] ${tool} cycle and nanosec cost of core ${core_id}: $cost_cycle $cost_nanosec"
        # step-2: least measurable interval
        least_interval_file="${result_dir}/interval_${core_id}.txt"
        least_interval_test
        least_round=`sed -n '2p' ${least_interval_file}`
        least_interval_cycle=`sed -n '3p' ${least_interval_file}`
        least_interval_nanosec=`sed -n '4p' ${least_interval_file}`
        echo "[result-info] ${tool} least interval cycle and nanosec of core ${core_id}: $least_interval_cycle $least_interval_nanosec"
    done
}

main
