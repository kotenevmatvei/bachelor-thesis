#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-tast=4
#SBATCH --time=24:00:00

#SBATCH --output=R-%x.%j.out
#SBATCH --error=R-%x.%j.err

bash plot_all.sh
