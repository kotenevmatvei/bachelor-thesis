import numpy as np
import os
from matplotlib import pyplot as plt

fig, ax = plt.subplots(figsize=(25, 10), dpi=300)

with open("data/diffusion_hist_refl.txt", "r") as f:
    lines = f.readlines()

counts_list = [np.fromstring(line, sep=" ") for line in lines[0::100]]
bin_bounds_list = [np.fromstring(line, sep=" ") for line in lines[1::100]]

print(f"lines_length = {len(lines)}")
print(f"counts_list length = {len(counts_list)}")
print(f"bin_bounds_list length = {len(bin_bounds_list)}")

for i, (counts, bin_bounds) in enumerate(zip(counts_list, bin_bounds_list)):
    # ax.plot(np.linspace(-1,1,100), counts)
    # print(f"processing historgram {i}")
    total_count = sum(counts)
    bin_width = bin_bounds[1] - bin_bounds[0]
    density = [c / (total_count * bin_width) for c in counts]

    centers = [(bin_bounds[j] + bin_bounds[j + 1]) / 2 for j in range(len(counts))]

    ax.bar(centers, density, width=bin_width, linewidth=1)


# first write to a tmp image to avoid race condition with imv
temp_path = "figures/diffusion_trajectories_tmp.png"
final_path = "figures/diffusion_trajectories.png"

fig.savefig(temp_path)

os.replace(temp_path, final_path)
