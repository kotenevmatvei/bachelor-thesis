import numpy as np
import os
from matplotlib import pyplot as plt

fig, axes = plt.subplots(figsize=(25, 10), dpi=300)

print(os.getcwd())

diffusion_trajectries = np.loadtxt("data/diffusion_trajectories.txt")

for traj in diffusion_trajectries:
    plt.plot(traj)

plt.title("this is new plot with TITLE")

plt.savefig("figures/diffusion_trajectories.png")
