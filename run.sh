#!/bin/bash
set -e

CONFIG_NAME=${1:-config}
echo "Using config: $CONFIG_NAME"

cd build/
echo "running the simulation..."
./tripple_diffusion "$CONFIG_NAME"
echo "Simulation ran successfully"
cd ..
echo "running the visualizations..."
python plot_tripple_diffusion.py "$CONFIG_NAME"
echo "the run done!"
