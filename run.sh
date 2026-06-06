#!/bin/bash
set -e

echo "building the project..."
make
cd build/

CONFIG_NAME=${1:-config}
echo "Using config: $CONFIG_NAME"

echo "running the simulation..."
./double_diffusion "$CONFIG_NAME"
cd ..
echo "running the visualizations..."
python plot_double_diffusion.py "$CONFIG_NAME"
echo "the run done!"
