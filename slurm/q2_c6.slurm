#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=200:00:00
#SBATCH--output=slurm/R-%x.%j.out

#SBATCH--error=slurm/R-%x.%j.err
export OMP_NUM_THREADS=4
export OMP_PLACES=cores

export OMP_PROC_BIND=spread
bash server_run.sh q2_c6
