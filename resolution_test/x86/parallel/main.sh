#!/bin/sh

for tool in PERFHOUND; do
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

#!/bin/bash

# some variables
mode=TS
kernel=ADD
bind_core=3
tool=PERFHOUND
arrlen=720896
measure_num=10000
src_file=test_x86_serial.c
output_dir=""

cost_cycle=0
cost_nanosec=0
least_round=0
least_interval_cycle=0
least_interval_nanosec=0
dcyle=0
dnanosec=0

if [ ${tool} == PAPI ]; then
    link_lib="-lpfh_papi_mpi"
    backend="papi"
else
    link_lib="-lpfh_mpi"
    backend="pfhd"
fi

function compile_and_run() {
    round=${1}
    exe=./${tool}_${mode}_${kernel}_${round}.x
    flags="-O3 -std=gnu99 -D$tool -DMODE=$mode -DKNAME=$kernel -DROUND=$round -DARRLEN=$arrlen -DNMEASURE=$measure_num"
    echo "tool=${tool}, mode=${mode}, kernel=${kernel}, round=${round}, arrlen=${arrlen}"
    gcc ${flags} -o $exe $src_file $link_lib
    taskset -c 3 ${exe}
    rm ${exe}
    output_dir="./data/${backend}/${mode}_${kernel}_test_6248_20220222/round_${round}/run_1"
}

function timing_cost_test() {
    compile_and_run 0
    # process data
    python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f timing_cost
    tmp_file="tmp_cost.txt"
    cost_cycle=`sed -n '1p' $tmp_file`
    cost_nanosec=`sed -n '2p' $tmp_file`
    rm $tmp_file
    echo "cycle and nanosec cost of timing instruction: $cost_cycle $cost_nanosec"
}


function least_interval_test() {
    i=$1
    step2_end="no"
    tmp_file="tmp_least_interval.txt"
    while [[ ${step2_end} == "no" ]]
    do
        compile_and_run $i
        # process data
        python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f least_interval -a ${i},${cost_cycle},${cost_nanosec}
        step2_end=`cat ${tmp_file}`
        let i=i+1
    done
    let least_round=i-1
    least_interval_cycle=`sed -n '2p' $tmp_file`
    least_interval_nanosec=`sed -n '3p' $tmp_file`
    echo "least interval cycle and nanosec: $least_interval_cycle $least_interval_nanosec"
    rm $tmp_file
}

function variation_resolution() {
    let i=least_round+1
    step3_end="no"
    tmp_file="tmp_var_resolution.txt"
    while [[ ${step3_end} == "no" ]]
    do
        compile_and_run $i
        # process data
        python algo.py -s 1,1 -e 1,2 -i ${output_dir} -b ${backend} -f variation_resolution -a ${least_round},${cost_cycle},${cost_nanosec}
        step3_end=`cat ${tmp_file}`
        let i=i+1
    done
    dcycle=`sed -n '2p' $tmp_file`
    dnanosec=`sed -n '3p' $tmp_file`
    echo "cycle and nanosec variation resolution: $dcycle $dnanosec"
    rm $tmp_file
}

# step-0: fix the frequency
sudo cpupower frequency-set -g performance -u 2.6G -d 2.6G
# step-1: timing instruction cost test
timing_cost_test
# step-2: least measurable interval
least_interval_test 700
# step-3: variation resolution
variation_resolution
# output result
echo "cycle and nanosec cost of timing instruction: $cost_cycle $cost_nanosec"
echo "least interval cycle and nanosec: $least_interval_cycle $least_interval_nanosec"
echo "cycle and nanosec variation resolution: $dcycle $dnanosec"