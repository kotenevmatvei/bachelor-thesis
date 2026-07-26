#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=10
#SBATCH --time=8:00:00

export OMP_NUM_THREADS=4
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

echo "activating venv"
source ./.venv/bin/activate
echo "venv activated"
echo "running the visualisation"
python plot_tripple_diffusion.py symmetric_success_1_c17
