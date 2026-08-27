#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-tast=4

#SBATCH --output=slurm/plot_all/R-%x.%j.out
#SBATCH --error=slurm/plot_all/R-%x.%j.err

bash plot_all.sh
