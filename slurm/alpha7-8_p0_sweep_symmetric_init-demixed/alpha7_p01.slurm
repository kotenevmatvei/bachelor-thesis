#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH--output=slurm/alpha7-8_p0_sweep_symmetric_init-demixed/R-%x.%j.out
#SBATCH--error=slurm/alpha7-8_p0_sweep_symmetric_init-demixed/R-%x.%j.err

bash server_run.sh "configs/alpha7-8_p0_sweep_symmetric_init-demixed/alpha7_p01.txt"
