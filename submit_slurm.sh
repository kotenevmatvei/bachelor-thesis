#!/bin/bash
for run in $(ls slurm); do
    for config in $(ls slurm/$run/*.slurm); do
        sbatch $config
    done
done
