#!/bin/bash

# variables for test
mode=TS
tool=papi
kernel=ADD
bind_core=12
measure_num=2000
tmin_check_num=30
tdiff_test_num=40
# param_arrlen = 2048
# param_arrlen = 65536
# param_arrlen = 1802240
param_arrlen=7208960
src_file=test_x86_serial.c


function compile_and_run() {
    arrlen=${1}
    cydelay=${2}
    if [ ! ${3} ]; then
        run_id="1"
    else
        run_id=${3}
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

function tcost_test() {
    compile_and_run 0 0
    # get the cost of timing instruction
    python algo2.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f cost_measure -a ${tcost_file}
}

function tmin_check() {
    arrlen=${1}
    cydelay=${2} # round is the same as cydelay
    for j in `seq 2 $[tmin_check_num + 1]`
    do
        compile_and_run ${arrlen} ${cydelay} ${j}
        python algo2.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f least_interval -a ${cydelay},${tcost_file},${tmin_file}
        passed=`cat ${tmin_file}`
        if [[ ${passed} == "no" ]]; then
            break
        fi
    done
}

function tmin_test() {
    end=0
    start=0
    cydelay=0
    test_step=10000
    arrlen=${1}
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let cydelay=cydelay+test_step
            compile_and_run ${arrlen} ${cydelay}
            python algo2.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f least_interval -a ${cydelay},${tcost_file},${tmin_file}
            passed=`cat ${tmin_file}`
            if [[ ${passed} == "no" ]]; then
                let start=cydelay
            else
                tmin_check ${arrlen} ${cydelay}
                check_passed=`cat ${tmin_file}`
                if [[ ${check_passed} == "no" ]]; then
                    passed="no"
                    let start=cydelay
                else
                    let end=cydelay
                fi
            fi
        done
        let cydelay=start
        let test_step=test_step/10
    done
}

function tdiff_test() {
    k=0
    left=0
    right=0
    test_step=100
    tdiff_file=${1}
    arrlen=${2}
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let k=k+test_step
            let end_delay=tmin_delay+k*tdiff_test_num
            for i in `seq ${tmin_delay} ${k} ${end_delay}`
            do
                compile_and_run ${arrlen} ${i}
            done
            python algo2.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f variation_resolution -a ${k},${tmin_delay},${tcost_file},${tdiff_file}
            passed=`cat ${tdiff_file}`
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
    # step-0: fix the frequency
    sudo cpupower frequency-set -g performance -u 2.5G -d 2.5G
    for i in `seq 1 20`
    do
        result_dir="result_files_${i}"
        if [ ! -d ${result_dir} ]; then
            mkdir -p ${result_dir}
        else
            mv ${result_dir}/data .
        fi
        # step-1: measure cost test
        tcost_file="${result_dir}/${tool}_tcost.txt"
        # tcost_test
        cost_cycle=`sed -n '1p' ${tcost_file}`
        cost_nanosec=`sed -n '2p' ${tcost_file}`
        # step-2: least measurable interval
        tmin_file="${result_dir}/${tool}_arrlen_${param_arrlen}_tmin_check_${tmin_check_num}.txt"
        # tmin_test ${param_arrlen}
        tmin_delay=`sed -n '2p' ${tmin_file}`
        tmin_cycle=`sed -n '3p' ${tmin_file}`
        tmin_nanosec=`sed -n '4p' ${tmin_file}`
        # step-3: variation resolution
        cy_tdiff_file="${result_dir}/${tool}_arrlen_${param_arrlen}_cycle_tdiff_${tdiff_test_num}.txt"
        ns_tdiff_file="${result_dir}/${tool}_arrlen_${param_arrlen}_nanosec_tdiff_${tdiff_test_num}.txt"
        tdiff_test ${cy_tdiff_file} ${param_arrlen}
        tdiff_test ${ns_tdiff_file} ${param_arrlen}
        tdiff_cycle=`sed -n '2p' ${cy_tdiff_file}`
        tdiff_nanosec=`sed -n '2p' ${ns_tdiff_file}`
        # output result
        echo "[result-info] ${tool} cycle and nanosec meausre cost: $cost_cycle $cost_nanosec"
        echo "[result-info] ${tool} cycle and nanosec least interval: $tmin_cycle $tmin_nanosec"
        echo "[result-info] ${tool} cycle and nanosec variation resolution: $tdiff_cycle $tdiff_nanosec"
        mv data ${result_dir}
    done
}

main
