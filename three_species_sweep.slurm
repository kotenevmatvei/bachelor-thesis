#!/usr/bin/env bash
#SBATCH --job-name=three_species_sweep
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --time=04:00:00
#SBATCH --mem=16G
#SBATCH --array=1-20%4
#SBATCH --output=slurm-%x-%A_%a.out
#SBATCH --error=slurm-%x-%A_%a.err

set -euo pipefail

# Submit from the project root with, for example:
#   sbatch --array=1-$(grep -cvE '^[[:space:]]*($|#)' sweep_configs.txt)%4 three_species_sweep.slurm
#
# sweep_configs.txt must contain one configuration basename per nonempty line,
# without ../configs/ and without .txt. Example:
#   q3_c2_rs0
#   q3_c3_rs0
#   q3_c3_rs1
#   q3_c3_rs2

PROJECT_ROOT="${SLURM_SUBMIT_DIR}"
CONFIG_LIST="${PROJECT_ROOT}/sweep_configs.txt"
TASK_ID="${SLURM_ARRAY_TASK_ID}"

if [[ ! -f "${CONFIG_LIST}" ]]; then
    echo "Missing configuration list: ${CONFIG_LIST}" >&2
    exit 1
fi

# Ignore blank lines and lines beginning with #.
CONFIG_NAME=$(grep -vE '^[[:space:]]*($|#)' "${CONFIG_LIST}" | sed -n "${TASK_ID}p")
if [[ -z "${CONFIG_NAME}" ]]; then
    echo "No configuration for array task ${TASK_ID}; exiting." >&2
    exit 0
fi

cd "${PROJECT_ROOT}/src"
mkdir -p ../data

# If your cluster needs a GSL module, load it here. Keep the same GSL version
# used for your validated CPU implementation.
# module load gsl

# Build once per job in the node-local temporary directory to avoid a race in
# which simultaneous array tasks overwrite a shared executable.
EXECUTABLE="${SLURM_TMPDIR:-/tmp}/three_species_cpu_${SLURM_JOB_ID}_${TASK_ID}"
gcc -std=c11 -O3 -march=native -fopenmp \
    simulation_openmp_corrected.c io.c utils.c \
    -lgsl -lgslcblas -lm -o "${EXECUTABLE}"

export OMP_NUM_THREADS="${SLURM_CPUS_PER_TASK}"
export OMP_PROC_BIND=close
export OMP_PLACES=cores
export OMP_DYNAMIC=false

echo "host=$(hostname)"
echo "config=${CONFIG_NAME}"
echo "OMP_NUM_THREADS=${OMP_NUM_THREADS}"
echo "SLURM_ARRAY_JOB_ID=${SLURM_ARRAY_JOB_ID}"
echo "SLURM_ARRAY_TASK_ID=${TASK_ID}"

"${EXECUTABLE}" "${CONFIG_NAME}"
