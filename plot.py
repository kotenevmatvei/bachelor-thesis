import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

P_vals = []
with open("data/theor_curves.txt") as file:
    for i, line in enumerate(file):
        if "V_axis" in line:
            V_axis = [float(v) for v in next(file).split(" ")]
            continue
        if line.startswith("#"): continue
        P_vals.append([float(p) for p in line.split(" ")])


datasets = []
with open("data/v_values_simple.txt") as file:
    for line in file:
        datasets.append([float(v_ij) for v_ij in line.split(" ")])

fig = plt.figure(figsize=(22, 8))

gs = gridspec.GridSpec(4, 2, width_ratios=[2, 1], wspace=0.15, hspace=0.1)

ax_traj = fig.add_subplot(gs[:, 0])

delta_t = 0.01
time_steps = len(datasets[0])
t_axis = [i * delta_t for i in range(time_steps)]

# plot the first 6 realizations
for dataset in datasets[:6]:
    ax_traj.plot(t_axis, dataset, linewidth=1)

ax_traj.set_xlabel("t", fontsize=14)
ax_traj.set_ylabel("v", fontsize=14)
ax_traj.set_xlim(0, 5)
ax_traj.set_ylim(-1.5, 1.8)
ax_traj.text(-0.05, 1.02, "(a)", transform=ax_traj.transAxes, fontsize=16,
             fontweight='bold')

times = [30, 50, 100, 400]
labels = ["t=0.3", "t=0.5", "t=1.0", "t=4.0"]

# read counts and bin bounds
hist_data = []
with open("data/hist_data.txt") as file:
    for line in file:
        if line.startswith("#"):
            hist_data.append({})
            hist_data[-1]["label"] = line[2:]
            continue
        hist_data[-1]["counts"] = [float(count) for count in line.split(" ")]
        next_line = next(file)
        hist_data[-1]["bin_bounds"] = [float(bin_bound) for bin_bound in next_line.split(" ")]


for i, hist in enumerate(hist_data):
    ax_hist = fig.add_subplot(gs[i, 1])

    label = hist["label"]
    bin_bounds = hist["bin_bounds"]
    counts = hist["counts"]

    total_count = sum(counts)
    bin_width = bin_bounds[1] - bin_bounds[0]
    density = [c / (total_count * bin_width) for c in counts]

    centers = [(bin_bounds[j] + bin_bounds[j + 1]) / 2 for j in range(len(counts))]

    ax_hist.bar(centers, density, width=bin_width, edgecolor="blue", color="grey",
                linewidth=1)

    ax_hist.text(0.1, 0.7, label, transform=ax_hist.transAxes, fontweight='bold',
                 backgroundcolor='yellow')

    ax_hist.set_ylabel("P(v,t)", fontsize=10)
    ax_hist.set_xlim(-1.5, 1.8)

    plt.plot(V_axis, P_vals[i], c="red", label="theory")

    # remove x-axis tick labels for the top 3 plots to avoid clutter
    if i < len(times) - 1:
        ax_hist.set_xticklabels([])
    else:
        ax_hist.set_xlabel("V", fontsize=14)

    # add (b) label only to the top histogram
    if i == 0:
        ax_hist.text(-0.1, 1.15, "(b)", transform=ax_hist.transAxes, fontsize=16,
                     fontweight='bold')

plt.savefig("figures/combined_figure.png", dpi=150)