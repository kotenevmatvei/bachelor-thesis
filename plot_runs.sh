#!/bin/bash
runs="
pow_sym_ref_init-dem_q2_c7_dt1e-05_nr1000000_rs0_bins100_cnts-step1000
pow_sym_ref_init-un_q2_c7_dt1e-05_nr1000000_rs0_bins100_cnts-step1000
pow_sym_ref_init-un_q2_c6_dt1e-08_nr1000000_rs0_bins100_cnts-step1000
pow_sym_ref_init-un_q2_c7_dt1e-08_nr1000000_rs0_bins100_cnts-step1000
pow_sym_ref_init-dem_q2_c6_dt1e-08_nr1000000_rs0_bins100_cnts-step1000
pow_sym_ref_init-dem_q2_c7_dt1e-08_nr1000000_rs0_bins100_cnts-step1000
"
for run in $runs; do
    echo plotting $run
    python plot_tripple_diffusion.py configs/$run.txt
done
