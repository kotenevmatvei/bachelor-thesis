#!/bin/bash
set -e

# for the server we fix the number of OMP threads to the optimal 5
# (anything bigger results in more communication overhead -> slower)

# set the number of omp threads
export OMP_NUM_THREADS=10

# cd /home/matvei/Desktop/bachelor-thesis/
echo "building the project..."
make
cd build/

CONFIG_NAME=${1:-config}
echo "Using config: $CONFIG_NAME"

echo "running the simulation..."
./tripple_diffusion "$CONFIG_NAME"
cd ..
echo "running the visualizations..."
python plot_tripple_diffusion.py "$CONFIG_NAME"
echo "the run done!"
