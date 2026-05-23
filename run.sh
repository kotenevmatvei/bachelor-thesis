#!/bin/bash
set -e

echo "removing old data..."
# rm data/diffusion_hist_periodic.txt
# rm data/diffusion_hist_reflective.txt
echo "building the project..."
make
cd cmake-build-debug/
echo "running the simulation..."
./single_diffusion
cd ..
echo "running the visualizations..."
python plot_diffusion.py
echo "done!"
