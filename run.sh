#!/bin/bash
set -e

echo "building the project..."
make
cd build/
echo "running the simulation..."
./double_diffusion
cd ..
echo "running the visualizations..."
python plot_double_diffusion.py
echo "the run done!"
