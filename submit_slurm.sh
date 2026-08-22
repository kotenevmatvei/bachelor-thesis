#!/bin/bash
for run in $(ls slurm); do
    for config in $(ls slurm/$run); do
        echo slurm/$run/$config
    done
done
