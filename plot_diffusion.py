import numpy as np
import shutil
import concurrent.futures
import os
import subprocess
from matplotlib import pyplot as plt
import matplotlib.animation as animation
import functools
from tqdm import tqdm

BOUNDARY = "sticky_top_refl_bottom"


def draw_trajectories():
    plt.subplots(figsize=(25, 10), dpi=300)

    print(os.getcwd())

    diffusion_trajectries = np.loadtxt(f"data/diffusion_trajectories_{BOUNDARY}.txt")

    plt.grid()

    for traj in diffusion_trajectries:
        plt.plot(traj)

    plt.title(f"Trajectories: {BOUNDARY} boundaries")

    temp_path = f"figures/diffusion_trajectories_{BOUNDARY}_tmp.png"
    final_path = f"figures/diffusion_trajectories_{BOUNDARY}.png"

    plt.ylabel("Coordinate x")
    plt.xlabel("Time t")

    plt.savefig(temp_path)
    plt.close()

    os.replace(temp_path, final_path)


def draw_histogram(style: str = "line"):
    with open(f"data/diffusion_hist_{BOUNDARY}.txt", "r") as f:
        lines = f.readlines()

    counts_list = [np.fromstring(line, sep=" ") for line in lines[0::2]]
    bin_bounds_list = [np.fromstring(line, sep=" ") for line in lines[1::2]]

    counts_init = counts_list[0]
    bin_bounds_init = bin_bounds_list[0]
    total_count = sum(counts_init)
    bin_width = bin_bounds_init[1] - bin_bounds_init[0]

    density = [c / (total_count * bin_width) for c in counts_init]
    centers = [
        (bin_bounds_init[j] + bin_bounds_init[j + 1]) / 2
        for j in range(len(counts_init))
    ]

    fig, ax = plt.subplots()
    ax.set_xlim(-1, 1)
    ax.set_ylim(0, 20)
    ax.set_xlabel("Coordinate x")
    ax.set_ylabel("Counts")
    ax.set_title(f"Particle diffusion, {BOUNDARY} boundaries")
    ax.grid()

    def run_hist(frame_index, bars):
        counts = counts_list[frame_index]
        density = [c / (total_count * bin_width) for c in counts]

        for rect, new_height in zip(bars.patches, density):
            rect.set_height(new_height)
        return bars.patches

    def run_line(frame_index, line):
        counts = counts_list[frame_index]
        density = [c / (total_count * bin_width) for c in counts]
        line.set_data(centers, density)
        return (line,)

    if style == "line":
        (line,) = ax.plot(centers, density)
        run = functools.partial(run_line, line=line)
    elif style == "bars":
        bars = ax.bar(centers, density, width=bin_width, linewidth=1)
        run = functools.partial(run_hist, bars=bars)

    total_frames = len(counts_list)

    ani = animation.FuncAnimation(
        fig,
        run,
        frames=total_frames,
        blit=True,
        cache_frame_data=False,
    )

    # plt.show()

    writer = animation.FFMpegWriter(fps=10, metadata=dict(artist="Matvei"))

    with tqdm(total=total_frames, desc="Saving animation") as pbar:

        def update_progress(current_frame, total):
            pbar.update(1)

        ani.save(
            f"figures/diffusion_line_{BOUNDARY}.mp4",
            writer=writer,
            progress_callback=update_progress,
        )


# 1. Define render_frame at the TOP LEVEL of the file!
def render_frame(i, counts_list, total_count, bin_width, centers, style, boundary):
    fig, ax = plt.subplots(
        figsize=(8, 6), dpi=100
    )  # Fresh figure for the separate CPU core
    ax.set_xlim(-1, 1)
    ax.set_ylim(0, 7)
    ax.set_xlabel("Coordinate x")
    ax.set_ylabel("Counts")
    ax.set_title(f"Particle diffusion, {boundary} boundaries")
    ax.grid()

    counts = counts_list[i]
    density = [c / (total_count * bin_width) for c in counts]

    if style == "line":
        ax.plot(centers, density)
    else:
        ax.bar(centers, density, width=bin_width, linewidth=1)

    filename = f"tmp_frames/frame_{i:05d}.png"
    fig.savefig(filename)
    plt.close(fig)  # Prevent memory leaks


# 2. Your main function
def ffmpeg_direct_hist(style="bars"):
    BOUNDARY = "sticky_top_refl_bottom"

    # ... (Your existing data reading logic here) ...
    with open(f"data/diffusion_hist_{BOUNDARY}.txt", "r") as f:
        lines = f.readlines()

    counts_list = [np.fromstring(line, sep=" ") for line in lines[0::2]]
    bin_bounds_list = [np.fromstring(line, sep=" ") for line in lines[1::2]]

    counts_init = counts_list[0]
    bin_bounds_init = bin_bounds_list[0]
    total_count = sum(counts_init)
    bin_width = bin_bounds_init[1] - bin_bounds_init[0]
    centers = [
        (bin_bounds_init[j] + bin_bounds_init[j + 1]) / 2
        for j in range(len(counts_init))
    ]

    total_frames = len(counts_list)
    os.makedirs("tmp_frames", exist_ok=True)

    # 3. Bind all the constant data to the top-level function so only 'i' needs to change
    worker_func = functools.partial(
        render_frame,
        counts_list=counts_list,
        total_count=total_count,
        bin_width=bin_width,
        centers=centers,
        style=style,
        boundary=BOUNDARY,
    )

    print(f"\nRendering {total_frames} frames in parallel...")
    with concurrent.futures.ProcessPoolExecutor() as executor:
        print(f"\nNumber of cores we will now use: {executor._max_workers}\n")
        list(tqdm(executor.map(worker_func, range(total_frames)), total=total_frames))

    print("Stitching video...")
    mp4_path = f"figures/diffusion_line_{BOUNDARY}.mp4"
    ffmpeg_command = [
        "ffmpeg",
        "-y",
        "-framerate",
        "10",
        "-i",
        "tmp_frames/frame_%05d.png",
        "-r",
        "10",
        "-c:v",
        "libx264",
        "-pix_fmt",
        "yuv420p",
        mp4_path,
    ]

    try:
        result = subprocess.run(
            ffmpeg_command, capture_output=True, text=True, check=True
        )
        print("Done!")

    except subprocess.CalledProcessError as e:
        print(f"Error during video stitching! FFmpeg failed with code {e.returncode}.")
        print(f"FFmpeg Error Output:\\n{e.stderr}")

    print("Removing temp files")
    shutil.rmtree("tmp_frames")

    print("Done!")


def main():
    draw_trajectories()
    # draw_histogram(style="bars")
    ffmpeg_direct_hist(style="bars")


if __name__ == "__main__":
    main()
