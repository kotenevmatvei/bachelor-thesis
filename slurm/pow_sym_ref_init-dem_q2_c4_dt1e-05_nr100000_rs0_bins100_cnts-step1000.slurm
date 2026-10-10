#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH --output=slurm/R-%x.%j.out
#SBATCH --error=slurm/R-%x.%j.err

bash server_run.sh "configs/pow_sym_ref_init-dem_q2_c4_dt1e-05_nr100000_rs0_bins100_cnts-step1000.txt"
