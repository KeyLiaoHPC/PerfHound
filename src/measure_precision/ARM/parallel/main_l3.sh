#!/bin/bash

# variables for test
mode=TS
tool=papi
nprocs=128
kernel=ADD
measure_num=2000
tmin_check_num=30
tdiff_test_num=80
src_file=test_aarch64_parallel.c
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
    output_dir="./data/${tool}/nprocs_${nprocs}/arrlen_${arrlen}/${mode}_${kernel}_test_kp920/cydelay_${cydelay}/run_${run_id}"
    if [ ! -d ${output_dir} ]; then
        exe=./${tool}_${mode}_${kernel}_${cydelay}_mpi.x
        flags="-O3 -std=gnu99 -DTOOL=$tool -DMODE=$mode -DKNAME=$kernel -DCYDELAY=$cydelay -DARRLEN=$arrlen -DNMEASURE=$measure_num"
        echo "tool=${tool}, mode=${mode}, kernel=${kernel}, cycle_delay=${cydelay}, arrlen=${arrlen}"
        mpicc ${flags} -o $exe $src_file -lperfhound_mpi
        mpirun -np ${nprocs} --mca btl ^openib --bind-to core --map-by slot:pe=1 ${exe}
        rm ${exe}
    fi
}


function tcost_test() {
    compile_and_run ${1} 0 0
    # get the measure cost
    python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f cost_measure -a ${tcost_file}
}

function tmin_check() {
    nprocs=${1}
    arrlen=${2}
    cydelay=${3}
    for j in `seq 2 $[tmin_check_num + 1]`
    do
        compile_and_run ${nprocs} ${arrlen} ${cydelay} ${j}
        python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f least_interval -a ${cydelay},${tcost_file},${tmin_file}
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
    test_step=1000
    nprocs=${1}
    arrlen=${2}
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let cydelay=cydelay+test_step
            compile_and_run ${nprocs} ${arrlen} ${cydelay}
            python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f least_interval -a ${cydelay},${tcost_file},${tmin_file}
            passed=`cat ${tmin_file}`
            if [[ ${passed} == "no" ]]; then
                let start=cydelay
            else
                tmin_check ${nprocs} ${arrlen} ${cydelay}
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
    nprocs=${2}
    arrlen=${3}
    while [[ ${test_step} -gt 0 ]]
    do
        passed="no"
        while [[ ${passed} == "no" ]]
        do
            let k=k+test_step
            echo "try tdiff = ${k}"
            let cnt=0
            for i in `seq 1 ${tdiff_test_num}`
            do
                let delay_2=tmin_delay+i*k
                let delay_1=delay_2-k
                # compile_and_run ${nprocs} ${arrlen} ${delay_1}
                compile_and_run ${nprocs} ${arrlen} ${delay_2}
                python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${tool} -f variation_resolution -a ${k},${tcost_file},${tdiff_file}
                passed=`cat ${tdiff_file}`
                if [[ ${passed} == "no" ]]; then
                    echo "${delay_1} and ${delay_2} not passed"
                    break
                else
                    echo "${delay_1} and ${delay_2} passed"
                    let cnt=cnt+1
                fi
            done

            if [[ ${cnt} -eq ${tdiff_test_num} ]]; then
                echo "tdiff = ${k} passed"
                let right=k
            else
                let left=k
            fi
        done
        let k=left
        let test_step=test_step/10
    done
}

function main() {
    # step-0: fix the frequency
    sudo cpupower frequency-set -g performance -u 2.5G -d 2.5G
    for i in `seq 1 3`
    do
        result_dir="result_files_${i}"
        if [ ! -d ${result_dir} ]; then
            mkdir -p ${result_dir}
        else
            mv ${result_dir}/data .
        fi

        if [[ $nprocs -gt 64 ]]; then
            let factor=nprocs-64
        else
            let factor=nprocs
        fi
        # let single_core_arrlen=16777216
        # let param_arrlen=single_core_arrlen/factor
        let param_arrlen=131072

        # step-1: measure cost test
        tcost_file="${result_dir}/${tool}_cost.txt"
        # tcost_test ${nprocs}
        cost_cycle=`sed -n '1p' ${tcost_file}`
        cost_nanosec=`sed -n '2p' ${tcost_file}`
        # step-2: least measurable interval
        tmin_file="${result_dir}/${tool}_arrlen_${param_arrlen}_tmin_check_${tmin_check_num}.txt"
        tmin_test ${nprocs} ${param_arrlen}
        tmin_delay=`sed -n '2p' ${tmin_file}`
        tmin_cycle=`sed -n '3p' ${tmin_file}`
        tmin_nanosec=`sed -n '4p' ${tmin_file}`
        # # step-3: variation resolution
        cy_tdiff_file="${result_dir}/${tool}_arrlen_${param_arrlen}_cycle_tdiff.txt"
        ns_tdiff_file="${result_dir}/${tool}_arrlen_${param_arrlen}_nanosec_tdiff.txt"
        tdiff_test ${cy_tdiff_file} ${nprocs} ${param_arrlen}
        tdiff_test ${ns_tdiff_file} ${nprocs} ${param_arrlen}
        tdiff_cycle=`sed -n '2p' ${cy_tdiff_file}`
        tdiff_nanosec=`sed -n '2p' ${ns_tdiff_file}`
        # output result
        echo "[result-info] ${tool} cycle and nanosec tcost: $cost_cycle $cost_nanosec"
        echo "[result-info] ${tool} cycle and nanosec tmin: $tmin_cycle $tmin_nanosec"
        echo "[result-info] ${tool} cycle and nanosec tdiff: $tdiff_cycle $tdiff_nanosec"
        mv data ${result_dir}
    done
}

main
