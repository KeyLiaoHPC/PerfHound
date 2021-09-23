#!/bin/bash

#SBATCH --job-name=pfhsim        # 作业名
#SBATCH --partition=cpu        # cpu 队列
#SBATCH -n 320                 # 总核数 160
#SBATCH --ntasks-per-node=40   # 每节点核数
#SBATCH --output=%j.out
#SBATCH --error=%j.err

module load gcc/9.2.0-gcc-4.8.5
module load openmpi/4.0.5-gcc-9.2.0
source ~/hjzhang/scripts/gsl-2.7.sh
source ~/hjzhang/scripts/openblas-0.3.12.sh

mpirun -n 320 ./pfhsim
