#!/bin/bash
for run in $(ls slurm); do
    for config in $(ls slurm/$run); do
        sbatch slurm/$run/$config
    done
done
