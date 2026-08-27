#!/bin/bash
for run in $(ls configs); do
    for config in $(ls configs/$run); do
        python plot_tripple_diffusion.py $run/$config
    done
done
