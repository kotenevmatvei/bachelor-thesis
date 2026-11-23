#!/bin/bash

# calculated lower level only from middle peaks (now averaging)
declare -A c_lmh_table=(
    ["5"]="0.11 0.389639 1.66086"
    ["5.2"]="0.1095 0.459128 1.92818"
    ["5.3"]="0.1055 0.448945 1.86573"
    ["5.33"]="0.10575 0.447026 1.89393"
    ["5.5"]="0.1085 0.44532 1.87991"
    ["5.6"]="0.1065 0.435034 1.78424"
    ["6"]="0.1045 0.431879 1.78323"
    ["7"]="0.091 0.416074 1.6994"
    ["9"]="0.5 0.5 0.5"
)

for run_path in $(ls "/home/matvei/bachelor-thesis/c-slide-sym-ref/q2/dt1e-05/nr100000/dem/"); do
    echo Run path is: $run_path
    run_name=$(basename $run_path)
    echo Run name is: $run_name

    c=${run_name#*_c}
    c=${c%%_*}

    echo "c = $c"
    echo "${c_lmh_table[$c]}"
    read -r l m h <<< "${c_lmh_table[$c]}"

    echo "l = $l"
    echo "m = $m"
    echo "h = $h"


    python plot_tripple_diffusion.py configs/$run_name.txt -tph $h -tpl $m -rf c-slide-sym-ref/q2/dt1e-05/nr100000/dem/
    # check if the animations for this run have already been generated
    # if [ "$(ls runs/$run_name/animations)" ]; then
    #     printf "$run_name already has animations\n\n"
    # else
    #     printf "$run_name does not have animations yet\n\n"
    #     python plot_tripple_diffusion.py configs/$run_name.txt
    # fi
done
