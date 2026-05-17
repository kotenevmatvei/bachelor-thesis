#!/bin/bash
set -e

echo "removing old data..."
rm data/diffusion_hist_refl.txt
echo "building the project..."
make
cd cmake-build-debug/
echo "running the simulation..."
./single_diffusion
cd ..
echo "running the visualizations..."
python plot_diffusion.py
echo "done!"
