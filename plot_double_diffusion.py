import numpy as np
import concurrent.futures
import os
import subprocess
from matplotlib import pyplot as plt
import matplotlib.animation as animation
import functools
from tqdm import tqdm

BOUNDARY = "sticky_top_refl_bottom"

centers = np.linspace(-1, 1, 100)

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


def render_frame(i, A_counts_list, B_counts_list, boundary):
    fig, ax = plt.subplots(
        figsize=(8, 6), dpi=100
    )
    ax.set_xlim(-1, 1)
    ax.set_ylim(0, 40)
    ax.set_xlabel("Coordinate x")
    ax.set_ylabel("Counts")
    ax.set_title(f"Particle diffusion, {boundary} boundaries")
    ax.grid()

    A_counts = A_counts_list[i]
    B_counts = B_counts_list[i]

    ax.plot(centers, A_counts)
    ax.plot(centers, B_counts)

    filename = f"tmp_frames/frame_{i:05d}.png"
    fig.savefig(filename)
    plt.close(fig)


def ffmpeg_direct_hist(style="bars"):

    with open("data/double_diffusion_counts.txt", "r") as f:
        lines = f.readlines()

    A_counts_list = [np.fromstring(line, sep=" ") for line in lines[0::10]]
    B_counts_list = [np.fromstring(line, sep=" ") for line in lines[1::10]]

    A_counts_init = A_counts_list[0]
    B_counts_init = B_counts_list[0]

    total_frames = len(A_counts_list)

    os.makedirs("tmp_frames", exist_ok=True)

    worker_func = functools.partial(
        render_frame,
        A_counts_list=A_counts_list,
        B_counts_list=B_counts_list,
        boundary=BOUNDARY,
    )

    print(f"\nRendering {total_frames} frames in parallel...")
    with concurrent.futures.ProcessPoolExecutor() as executor:
        print(f"\nNumber of cores we will now use: {executor._max_workers}\n")
        list(tqdm(executor.map(worker_func, range(total_frames)), total=total_frames))

    print("Stitching video...")
    mp4_path = f"figures/double_diffusion_{BOUNDARY}.mp4"
    ffmpeg_command = [
        "ffmpeg",
        "-y",
        "-i",
        "tmp_frames/frame_%05d.png",
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
        print("Animation done!")

    except subprocess.CalledProcessError as e:
        print(f"Error during video stitching! FFmpeg failed with code {e.returncode}.")
        print(f"FFmpeg Error Output:\\n{e.stderr}")



def main():
    # draw_trajectories()
    # draw_histogram(style="bars")
    ffmpeg_direct_hist(style="bars")


if __name__ == "__main__":
    main()
