import numpy as np
from matplotlib import pyplot as plt
import matplotlib.animation as animation
import functools
from tqdm import tqdm

with open("data/diffusion_hist_refl.txt", "r") as f:
    lines = f.readlines()

counts_list = list([np.fromstring(line, sep=" ") for line in lines[0::2]])
bin_bounds_list = list([np.fromstring(line, sep=" ") for line in lines[1::2]])


def data_gen():
    for counts, bin_bounds in zip(counts_list, bin_bounds_list):
        yield counts, bin_bounds


counts_init = counts_list[0]
bin_bounds_init = bin_bounds_list[0]
total_count = sum(counts_init)
bin_width = bin_bounds_init[1] - bin_bounds_init[0]

density = [c / (total_count * bin_width) for c in counts_init]
centers = [
    (bin_bounds_init[j] + bin_bounds_init[j + 1]) / 2 for j in range(len(counts_init))
]
fig, ax = plt.subplots()
ax.set_xlim(-1, 1)
ax.grid()
bars = ax.bar(centers, density, width=bin_width, linewidth=1)


def run(data, bars):
    # update the data
    counts, _ = data
    density = [c / (total_count * bin_width) for c in counts]

    for rect, new_height in zip(bars.patches, density):
        rect.set_height(new_height)

    return bars.patches


run_with_bars = functools.partial(run, bars=bars)

total_frames = len(counts_list)

ani = animation.FuncAnimation(
    fig, run_with_bars, data_gen, blit=True, save_count=total_frames
)

writer = animation.FFMpegWriter(fps=10, metadata=dict(artist="Matvei"))

with tqdm(total=total_frames, desc="Saving animation") as pbar:

    def update_progress(current_frame, total):
        pbar.update(1)

    ani.save(
        "figures/diffusion_refl.mp4",
        writer=writer,
        progress_callback=update_progress,
    )
