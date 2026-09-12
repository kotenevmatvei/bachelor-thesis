#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH --output=slurm/pow_sym_ref_init-un-08_nr1000000_rs0_bins100_cnts-step1/R-%x.%j.out
#SBATCH --error=slurm/pow_sym_ref_init-un-08_nr1000000_rs0_bins100_cnts-step1/R-%x.%j.err

bash server_run.sh "configs/pow_sym_ref_init-un-08_nr1000000_rs0_bins100_cnts-step1.txt"
