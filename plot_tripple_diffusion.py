import numpy as np
import concurrent.futures
import os
import shutil
import time
import subprocess
import argparse
from matplotlib import pyplot as plt
import functools
from tqdm import tqdm

from helpers import parse_config

BOUNDARY = "reflective"

centers = np.linspace(-1, 1, 100)

"""
NOT CURRENTLY USED:

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



"""


def render_frame(i, A_cnts_list, B_cnts_list, C_cnts_list, boundary, run):
    fig, ax = plt.subplots(figsize=(8, 6), dpi=100)
    ax.set_xlim(-1, 1)
    ax.set_ylim(0, 3)
    ax.set_xlabel("Coordinate x")
    ax.set_ylabel("Counts")
    ax.set_yticks(np.linspace(0, 3, 30))
    # ax.set_title(f"Particle diffusion, {boundary} boundaries")
    ax.grid(which="major")
    ax.grid(which="minor")

    A_cnts = A_cnts_list[i][1:]
    B_cnts = B_cnts_list[i][1:]
    C_cnts = C_cnts_list[i][1:]

    ax.plot(centers, A_cnts)
    ax.plot(centers, B_cnts)
    ax.plot(centers, C_cnts)

    filename = f"runs/{run}/tmp_frames/frame_{i:05d}.png"
    fig.savefig(filename)
    plt.close(fig)


def build_config_name(
    type_,
    dependency,
    run,
    boundary,
    init_density,
    delta_t,
    n_t,
    n_realizations,
    n_bins,
    upper_bound,
    lower_bound,
    c,
    q,
    p_0,
    alpha,
    rs,
    cnts_timestep,
):
    if type_ == "pow":
        config_name = (
            f"{type_}_{dependency}_{boundary}_init-{init_density}_q{q}_c{c:g}_dt{delta_t}"
            f"_nr{n_realizations}_rs{rs}_bins{n_bins}_ft{cnts_timestep}"
        )
    elif type_ == "log":
        config_name = (
            f"{type_}_{dependency}_{boundary}_init-{init_density}_p0{p_0:g}_alpha{alpha:g}_dt{delta_t}"
            f"_nr{n_realizations}_rs{rs}_bins{n_bins}_ft{cnts_timestep}"
        )
    else:
        raise ValueError(f"Unknown type {type_}")

    return config_name


def ffmpeg_direct_hist(
    type_,
    dependency,
    run,
    boundary,
    init_density,
    delta_t,
    n_t,
    n_realizations,
    n_bins,
    upper_bound,
    lower_bound,
    c,
    q,
    p_0,
    alpha,
    rs,
    cnts_timestep,
):
    config_name = build_config_name(
        type_,
        dependency,
        run,
        boundary,
        init_density,
        delta_t,
        n_t,
        n_realizations,
        n_bins,
        upper_bound,
        lower_bound,
        c,
        q,
        p_0,
        alpha,
        rs,
        cnts_timestep,
    )
    name = f"cnts_{config_name}"

    print(f"name: {name}")
    data_filename = f"runs/{run}/data/{name}.txt"

    with open(data_filename, "r") as f:
        lines = f.readlines()

    A_cnts_list = [np.fromstring(line, sep=" ") for line in lines[0::3]]
    B_cnts_list = [np.fromstring(line, sep=" ") for line in lines[1::3]]
    C_cnts_list = [np.fromstring(line, sep=" ") for line in lines[2::3]]

    # check if there is more frames in data than should be in one run. In this case we
    # are continuing a run and there might be multiple animations for the first stages
    # already generated. so we add a corresponding suffix to the animation name and only
    # regenerate the new data
    n_frames_in_run = int(n_t / cnts_timestep)
    suffix = str(int(len(A_cnts_list) / n_frames_in_run))

    A_cnts_list = A_cnts_list[-n_frames_in_run::1]
    B_cnts_list = B_cnts_list[-n_frames_in_run::1]
    C_cnts_list = C_cnts_list[-n_frames_in_run::1]

    print("A_cnts_list length: ", len(A_cnts_list))

    bin_width = (upper_bound - lower_bound) / n_bins

    # normalize the histograms
    A_cnts_list = [count / (n_realizations * bin_width) for count in A_cnts_list]
    B_cnts_list = [count / (n_realizations * bin_width) for count in B_cnts_list]
    C_cnts_list = [count / (n_realizations * bin_width) for count in C_cnts_list]

    total_frames = len(A_cnts_list)

    shutil.rmtree(f"runs/{run}/tmp_frames", ignore_errors=True)
    os.makedirs(f"runs/{run}/tmp_frames")

    worker_func = functools.partial(
        render_frame,
        A_cnts_list=A_cnts_list,
        B_cnts_list=B_cnts_list,
        C_cnts_list=C_cnts_list,
        boundary=BOUNDARY,
        run=run,
    )

    render_start = time.time()

    print(f"\nRendering {total_frames} frames in parallel...")
    with concurrent.futures.ProcessPoolExecutor() as executor:
        executor.map(worker_func, range(total_frames))

    print("Stitching video...")
    animation_filename = f"runs/{run}/animations/{name}_iteration{suffix}.mp4"
    mp4_path = animation_filename
    ffmpeg_command = [
        "ffmpeg",
        "-y",
        "-i",
        f"runs/{run}/tmp_frames/frame_%05d.png",
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
    render_time = time.time() - render_start
    print(f"Rendering took {render_time}s")


def main():
    parser = argparse.ArgumentParser(description="Get config name")
    parser.add_argument("config_name", help="The name of the config file")
    args = parser.parse_args()
    config_name = args.config_name
    config = parse_config(config_name)

    type_ = config["type"]
    run = config["run"]
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
    p_0 = config["p_0"]
    alpha = config["alpha"]
    cnts_timestep = config["cnts_timestep"]
    dependency = config["dependency"]
    boundary = config["boundary"]
    init_density = config["init_density"]
    print("Config: ")
    print(config)

    # draw_trajectories()
    # draw_histogram(style="bars")
    ffmpeg_direct_hist(
        type_=type_,
        dependency=dependency,
        run=run,
        boundary=boundary,
        init_density=init_density,
        delta_t=delta_t,
        n_t=n_t,
        n_bins=n_bins,
        n_realizations=n_realizations,
        upper_bound=upper_bound,
        lower_bound=lower_bound,
        c=c,
        q=q,
        p_0=p_0,
        alpha=alpha,
        rs=rs,
        cnts_timestep=cnts_timestep,
    )


if __name__ == "__main__":
    main()
