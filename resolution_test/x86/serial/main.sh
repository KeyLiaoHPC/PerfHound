#!/bin/bash

# variables for test
mode=TS
kernel=ADD
bind_core=7
tool=PERFHOUND
arrlen=720896
measure_num=10000
src_file=test_x86_serial.c
check_times=20
resolution_test_num=30


function compile_and_run() {
    round=${1}
    if [ ! ${2} ]; then
        run_id="1"
    else
        run_id=${2}
    fi
    output_dir="./data/${backend}/arrlen_${arrlen}/${mode}_${kernel}_test_6248_20220222/round_${round}/run_${run_id}"
    if [ ! -d ${output_dir} ]; then
        exe=./${tool}_${mode}_${kernel}_${round}.x
        flags="-O3 -std=gnu99 -D$tool -DMODE=$mode -DKNAME=$kernel -DROUND=$round -DARRLEN=$arrlen -DNMEASURE=$measure_num"
        echo "tool=${tool}, mode=${mode}, kernel=${kernel}, round=${round}, arrlen=${arrlen}"
        gcc ${flags} -o $exe $src_file $link_lib
        taskset -c ${bind_core} ${exe}
        rm ${exe}
    fi
}

function cost_measure_test() {
    compile_and_run 0
    # get the cost of timing instruction
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
    test_step=10000
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

function variation_resolution() {
    k=0
    left=0
    right=0
    test_step=100
    resolution_file=$1
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let k=k+test_step
            let end_round=least_round+k*resolution_test_num
            for i in `seq ${least_round} ${k} ${end_round}`
            do
                compile_and_run $i
            done
            python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f variation_resolution -a ${k},${least_round},${cost_measure_file},${resolution_file}
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
    if [ ${tool} == PAPI ]; then
        link_lib="-lpfh_papi"
        backend="papi"
    else
        link_lib="-lpfh"
        backend="pfhd"
    fi

    result_dir="test_results"
    if [ ! -d ${result_dir} ]; then
        mkdir -p ${result_dir}
    fi

    # step-0: fix the frequency
    sudo cpupower frequency-set -g performance -u 2.6G -d 2.6G
    # step-1: measure cost test
    cost_measure_file="${result_dir}/cost.txt"
    cost_measure_test
    cost_cycle=`sed -n '1p' ${cost_measure_file}`
    cost_nanosec=`sed -n '2p' ${cost_measure_file}`
    # step-2: least measurable interval
    least_interval_file="${result_dir}/interval.txt"
    least_interval_test
    least_round=`sed -n '2p' ${least_interval_file}`
    least_interval_cycle=`sed -n '3p' ${least_interval_file}`
    least_interval_nanosec=`sed -n '4p' ${least_interval_file}`
    # step-3: variation resolution
    cy_resolution_file="${result_dir}/cycle_resolution.txt"
    ns_resolution_file="${result_dir}/nanosec_resolution.txt"
    variation_resolution ${cy_resolution_file}
    variation_resolution ${ns_resolution_file}
    dcycle=`sed -n '2p' ${cy_resolution_file}`
    dnanosec=`sed -n '2p' ${ns_resolution_file}`
    # output result
    echo "[result-info] ${tool} cycle and nanosec cost of timing instruction: $cost_cycle $cost_nanosec"
    echo "[result-info] ${tool} least interval cycle and nanosec: $least_interval_cycle $least_interval_nanosec"
    echo "[result-info] ${tool} cycle and nanosec variation resolution: $dcycle $dnanosec"
}

main
