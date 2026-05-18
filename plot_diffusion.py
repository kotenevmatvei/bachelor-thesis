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
# bars = ax.bar(centers, density, width=bin_width, linewidth=1)
line, = ax.plot(centers, density)

def run_hist(data, bars):
    # update the data
    counts, _ = data
    density = [c / (total_count * bin_width) for c in counts]

    for rect, new_height in zip(bars.patches, density):
        rect.set_height(new_height)

    return bars.patches

def run_line(data, line):
    counts, _ = data
    density = [c / (total_count * bin_width) for c in counts]
    line.set_data(centers, density)
    return line,


run_line_partial = functools.partial(run_line, line=line)
# run_hist_partial = functools.partial(run_hist, bars=bars)

total_frames = len(counts_list)

ani = animation.FuncAnimation(
    fig,
    run_line_partial,
    data_gen,
    blit=True,
    save_count=total_frames,
    cache_frame_data=False,
)

writer = animation.FFMpegWriter(fps=10, metadata=dict(artist="Matvei"))

with tqdm(total=total_frames, desc="Saving animation") as pbar:

    def update_progress(current_frame, total):
        pbar.update(1)

    ani.save(
        "figures/diffusion_line_refl.mp4",
        writer=writer,
        progress_callback=update_progress,
    )
