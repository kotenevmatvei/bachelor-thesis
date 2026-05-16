import numpy as np
from matplotlib import pyplot as plt

fig, axes = plt.subplots(figsize=(16, 9), dpi=500)

diffusion_trajectries = np.loadtxt("data/diffusion_trajectories.txt")

for traj in diffusion_trajectries:
    plt.plot(traj)

plt.title("this is new plot with TITLE")

plt.savefig("figures/diffusion_trajectories.png")
