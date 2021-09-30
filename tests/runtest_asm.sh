#!/bin/bash

for k in ST FMA LD
do
    for n in {0..5000..2}
    do
        echo === $k x $n TS ===
        #NINS=$n NMEASURE=1050  KNAME=$k MODE=TS ./compile.sh
        mpirun -np 40 --map-by core --bind-to core ./test3_papi_${n}.x 
        mpirun -np 40 --map-by core --bind-to core ./test3_pfh_${n}.x 
    done
    mv ../data/pfh_papi ../data/pfh_0930_papi_ts_$k
    mv ../data/pfh ../data/pfh_0930_ts_$k

    #for n in {0..5000..2}
    #do
    #    echo === $k x $n EVX ===
    #    NINS=$n NMEASURE=1050 KNAME=$k MODE=EVX ./compile.sh
    #    mpirun -np 40 --map-by core --bind-to core ./test3_papi.x 
    #    mpirun -np 40 --map-by core --bind-to core ./test3_pfh.x 
    #done
    #mv ../data/pfh_papi ../data/pfh_0930_papi_evx_$k
    #mv ../data/pfh ../data/pfh_0930_evx_$k

done
