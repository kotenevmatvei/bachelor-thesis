#!/bin/bash
for i in $( ls slurm); do
    sbatch slurm/$i
done
