#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH --output=slurm/q2-3_c_sweep_cyclic_init-uniform/R-%x.%j.out
#SBATCH --error=slurm/q2-3_c_sweep_cyclic_init-uniform/R-%x.%j.err

bash server_run.sh "configs/q2-3_c_sweep_cyclic_init-uniform/q2_c5.1.txt"
