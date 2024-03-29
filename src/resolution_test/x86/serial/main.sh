#!/bin/bash

# some variables
mode=TS
kernel=ADD
bind_core=7
tool=PERFHOUND
arrlen=720896
measure_num=10000
src_file=test_x86_serial.c
output_dir=""
timing_cost_file="cost.txt"
resolution_file="resolution.txt"
least_interval_file="interval.txt"
resolution_continue_num=20

cost_cycle=0
cost_nanosec=0
least_round=0
least_interval_cycle=0
least_interval_nanosec=0
dcyle=0
dnanosec=0

if [ ${tool} == PAPI ]; then
    link_lib="-lpfh_papi"
    backend="papi"
else
    link_lib="-lpfh"
    backend="pfhd"
fi

function compile_and_run() {
    round=${1}
    output_dir="./data/${backend}/arrlen_${arrlen}/${mode}_${kernel}_test_6248_20220222/round_${round}/run_1"
    if [ ! -d ${output_dir} ]; then
        exe=./${tool}_${mode}_${kernel}_${round}.x
        flags="-O2 -std=gnu99 -D$tool -DMODE=$mode -DKNAME=$kernel -DROUND=$round -DARRLEN=$arrlen -DNMEASURE=$measure_num"
        echo "tool=${tool}, mode=${mode}, kernel=${kernel}, round=${round}, arrlen=${arrlen}"
        gcc ${flags} -o $exe $src_file $link_lib
        taskset -c ${bind_core} ${exe}
        rm ${exe}
    fi
}

function timing_cost_test() {
    compile_and_run 0
    # get the cost of timing instruction
    python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f timing_cost -a ${timing_cost_file}
    cost_cycle=`sed -n '1p' ${timing_cost_file}`
    cost_nanosec=`sed -n '2p' ${timing_cost_file}`
}

function least_interval_test() {
    i=0
    end=0
    start=0
    test_step=1000
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let i=i+test_step
            compile_and_run $i
            python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f least_interval -a ${i},${timing_cost_file},${least_interval_file}
            passed=`cat ${least_interval_file}`
            if [[ ${passed} == "no" ]]; then
                let start=i
            else
                let end=i
            fi
        done
        let i=start
        let test_step=test_step/10
    done
    least_round=`sed -n '2p' ${least_interval_file}`
    least_interval_cycle=`sed -n '3p' ${least_interval_file}`
    least_interval_nanosec=`sed -n '4p' ${least_interval_file}`
}

function variation_resolution() {
    k=0
    left=0
    right=0
    test_step=100
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let k=k+test_step
            let end_round=least_round+k*resolution_continue_num
            for i in `seq ${least_round} ${k} ${end_round}`
            do
                compile_and_run $i
            done
            python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f variation_resolution -a ${resolution_continue_num},${timing_cost_file},${least_interval_file},${resolution_file}
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
    dcycle=`sed -n '2p' ${resolution_file}`
    dnanosec=`sed -n '3p' ${resolution_file}`
}

# step-0: fix the frequency
sudo cpupower frequency-set -g performance -u 2.6G -d 2.6G
# step-1: timing instruction cost test
timing_cost_test
# step-2: least measurable interval
least_interval_test
# step-3: variation resolution
variation_resolution
# output result
echo "cycle and nanosec cost of timing instruction: $cost_cycle $cost_nanosec"
echo "least interval cycle and nanosec: $least_interval_cycle $least_interval_nanosec"
echo "cycle and nanosec variation resolution: $dcycle $dnanosec"