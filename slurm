#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=5
#SBATCH --time=8:00:00

export OMP_NUM_THREADS=4
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

bash run_tripple.sh symmetric_success_1_c17
