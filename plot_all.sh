#!/bin/bash
for run_path in $(ls -d configs/*/); do
    echo Run path is: $run_path
    run_name=$(basename $run_path)
    echo Run name is $run_name
    # check if the animations for this run have already been generated
    if [ "$(ls runs/$run_name/animations)" ]; then
        printf "$run_name already has animations\n\n"
    else
        printf "$run_name does not have animations yet\n\n"
        for config in $(ls configs/$run_name); do
            python plot_tripple_diffusion.py $run_name/$config
        done
    fi
done
