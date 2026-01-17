import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

# 1. Read Trajectory Data
datasets = []
with open("data/v_values.txt") as file:
    for line in file:
        if line.startswith("row"):
            datasets.append([])
        else:
            # Append to the last dataset added
            datasets[-1].append(float(line))

# Setup the Figure Layout
fig = plt.figure(figsize=(18, 8))
# Create a grid: 4 rows, 2 columns.
# The left column (trajectories) is slightly wider (width_ratios).
gs = gridspec.GridSpec(4, 2, width_ratios=[2, 1], wspace=0.15, hspace=0.1)

# --- Left Panel: Trajectories (a) ---
# This subplot spans all 4 rows (:) in the first column (0)
ax_traj = fig.add_subplot(gs[:, 0])

# Construct meaningful time axis (steps * delta_t)
delta_t = 0.01
time_steps = len(datasets[0])
t_axis = [i * delta_t for i in range(time_steps)]

# Plot the first 6 realizations
for dataset in datasets[:6]:
    ax_traj.plot(t_axis, dataset, linewidth=1)

ax_traj.set_xlabel("t", fontsize=14)
ax_traj.set_ylabel("v", fontsize=14)
ax_traj.set_xlim(0, 5)
ax_traj.set_ylim(-1.5, 1.8)
ax_traj.text(-0.05, 1.02, "(a)", transform=ax_traj.transAxes, fontsize=16, fontweight='bold')


# --- Right Panel: Stacked Histograms (b) ---
times = [30, 50, 100, 400]
# Labels based on t = steps * 0.01
labels = ["t=0.3", "t=0.5", "t=1.0", "t=4.0"]

for i, (time_step, label) in enumerate(zip(times, labels)):
    # Create subplot in the specific row i, second column (1)
    ax_hist = fig.add_subplot(gs[i, 1])

    # Read bounds
    bin_bounds = []
    with open(f"data/bounds_t{time_step}.txt") as file:
        for line in file:
            bin_bounds.append(float(line))

    # Read counts
    counts = []
    with open(f"data/counts_t{time_step}.txt") as file:
        for line in file:
            counts.append(int(line))

    # CALCULATION: Normalize Counts to Probability Density
    # Density = Count / (Total_N * Bin_Width)
    total_count = sum(counts)
    # Calculate bin width from the first bin (assuming uniform bins)
    bin_width = bin_bounds[1] - bin_bounds[0]
    density = [c / (total_count * bin_width) for c in counts]

    # Calculate centers for bar plotting
    centers = [(bin_bounds[j] + bin_bounds[j+1])/2 for j in range(len(counts))]

    # Plot bars
    ax_hist.bar(centers, density, width=bin_width, edgecolor="blue", color="grey", linewidth=1)

    # Styling
    # Add the yellow label
    ax_hist.text(0.1, 0.7, label, transform=ax_hist.transAxes, fontweight='bold', backgroundcolor='yellow')

    ax_hist.set_ylabel("P(v,t)", fontsize=10)
    ax_hist.set_xlim(-1.5, 1.8) # Match the range of the trajectory plot

    # Remove x-axis tick labels for the top 3 plots to avoid clutter
    if i < len(times) - 1:
        ax_hist.set_xticklabels([])
    else:
        ax_hist.set_xlabel("V", fontsize=14)

    # Add (b) label only to the top histogram
    if i == 0:
        ax_hist.text(-0.1, 1.15, "(b)", transform=ax_hist.transAxes, fontsize=16, fontweight='bold')

plt.savefig("plots/combined_figure.png", dpi=150)