import numpy as np
import concurrent.futures
import os
import shutil
import subprocess
import argparse
from matplotlib import pyplot as plt
import functools
from tqdm import tqdm

from helpers import parse_config

BOUNDARY = "reflective"

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


def render_frame(i, A_counts_list, B_counts_list, C_counts_list, boundary):
    fig, ax = plt.subplots(figsize=(8, 6), dpi=100)
    ax.set_xlim(-1, 1)
    ax.set_ylim(0, 3)
    ax.set_xlabel("Coordinate x")
    ax.set_ylabel("Counts")
    # ax.set_title(f"Particle diffusion, {boundary} boundaries")
    ax.grid()

    A_counts = A_counts_list[i][1:]
    B_counts = B_counts_list[i][1:]
    C_counts = C_counts_list[i][1:]

    ax.plot(centers, A_counts)
    ax.plot(centers, B_counts)
    ax.plot(centers, C_counts)

    filename = f"tmp_frames/frame_{i:05d}.png"
    fig.savefig(filename)
    plt.close(fig)


def ffmpeg_direct_hist(
    type_, delta_t, n_t, n_realizations, n_bins, upper_bound, lower_bound, c, q, rs, frame_timestep
):
    name = f"{type_}_counts_dt{delta_t}_nt{n_t}_nr{n_realizations}_c{c:g}_q{q}_rs{rs}_bins{n_bins}_ft{frame_timestep}"
    print(f"name: {name}")
    data_filename = f"data/{name}.txt"

    with open(data_filename, "r") as f:
        lines = f.readlines()

    A_counts_list = [np.fromstring(line, sep=" ") for line in lines[0::3]]
    B_counts_list = [np.fromstring(line, sep=" ") for line in lines[1::3]]
    C_counts_list = [np.fromstring(line, sep=" ") for line in lines[2::3]]

    bin_width = (upper_bound - lower_bound) / n_bins

    A_counts_list = [count / (n_realizations * bin_width) for count in A_counts_list]
    B_counts_list = [count / (n_realizations * bin_width) for count in B_counts_list]
    C_counts_list = [count / (n_realizations * bin_width) for count in C_counts_list]

    total_frames = len(A_counts_list)

    shutil.rmtree("tmp_frames", ignore_errors=True)
    os.makedirs("tmp_frames")

    worker_func = functools.partial(
        render_frame,
        A_counts_list=A_counts_list,
        B_counts_list=B_counts_list,
        C_counts_list=C_counts_list,
        boundary=BOUNDARY,
    )

    print(f"\nRendering {total_frames} frames in parallel...")
    with concurrent.futures.ProcessPoolExecutor() as executor:
        list(tqdm(executor.map(worker_func, range(total_frames)), total=total_frames))

    print("Stitching video...")
    animation_filename = f"animations/{name}.mp4"
    mp4_path = animation_filename
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
    parser = argparse.ArgumentParser(description="Get config name")
    parser.add_argument("config_name", help="The name of the config file")
    args = parser.parse_args()
    config_name = args.config_name
    config = parse_config(config_name)

    type = config["type"]
    delta_t = config["delta_t"]
    start = config["start"]
    upper_bound = config["upper_bound"]
    lower_bound = config["lower_bound"]
    d = config["d"]
    n_t = config["n_t"]
    n_realizations = config["n_realizations"]
    n_bins = config["n_bins"]
    c = config["c"]
    q = config["q"]
    rs = config["rs"]
    frame_timestep = config["frame_timestep"]
    print("Config: ")
    print(config)

    # draw_trajectories()
    # draw_histogram(style="bars")
    ffmpeg_direct_hist(
        type_=type,
        delta_t=delta_t,
        n_t=n_t,
        n_bins=n_bins,
        n_realizations=n_realizations,
        upper_bound=upper_bound,
        lower_bound=lower_bound,
        c=c,
        q=q,
        rs=rs,
        frame_timestep=frame_timestep
    )


if __name__ == "__main__":
    main()
